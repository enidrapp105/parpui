#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QObject>
#include <QQmlEngine>
#include <QtConcurrent>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QProcess>
#include <QDesktopServices>
#include <QColor>
#include <string.h>
#include <regex.h>
#include "parp.h"
#include "parpui.h"
#include "sqldatabase.h"




class Backend : public QObject{
    Q_OBJECT
    Q_PROPERTY(QStringList sounds READ sounds NOTIFY soundsChanged)
    Q_PROPERTY(QString virtual_mic_button_text READ virtual_mic_button_text  NOTIFY virtualmicToggle)
    //Q_PROPERTY(float volume READ volume WRITE volume_setter NOTIFY volumeChanged)
    //Q_PROPERTY(float indivvolume READ indivvolume WRITE indiv_volume_setter NOTIFY indivvolumeChanged)

public:
    Q_INVOKABLE void play(QString file_name);
    Q_INVOKABLE void load_sounds();
    Q_INVOKABLE void add_sound(QString file_path);
    Q_INVOKABLE void load_unload_devices();
    Q_INVOKABLE void remove_sound(QString file_path);
    Q_INVOKABLE void stop_all();
    Q_INVOKABLE void open_sounds_folder();

    Q_INVOKABLE void volume_setter(float volume){
        if(m_volume != volume){
            m_volume = volume;
        }
    }
    Q_INVOKABLE void indiv_volume_setter(float volume, QString file_path){
        Sound* sound = find_sound(file_path);
        if(sound->gain != volume){
            sound->gain = volume;
        }
    }
    Q_INVOKABLE void color_setter(QColor color, QString file_path){
        Sound* sound = find_sound(file_path);
        if(sound->button_color != color){
            sound->button_color = color;
        }
    }
    Q_INVOKABLE float sound_gain(QString file_path){
        Sound* sound = find_sound(file_path);
        return sound ? sound->gain : 1.0f;
    }
    float volume() const {return m_volume; }
    QStringList sounds() const {
        QStringList result;
        for (const Sound &s : m_sounds)
            result.append(s.display_path);
        return result;
    }
    QString virtual_mic_button_text() const {return m_virtual_mic_button_text; }
signals:
    void soundsChanged();
    void virtualmicToggle();

private:
    void register_sound(paTestData* d) {
        QMutexLocker lock(&m_active_mutex);
        m_active_sounds.append(d);
    }
    void unregister_sound(paTestData* d) {
        QMutexLocker lock(&m_active_mutex);
        m_active_sounds.removeOne(d);
    }
    Sound* find_sound(const QString &display_path){
        for(Sound &s : m_sounds)
            if(s.display_path == display_path) return &s;
        return nullptr;
    }
    float m_volume = 1.0f;
    QMap<QString, Sound> m_sounds;
    QString m_virtual_mic_button_text;
    QString m_sounds_path;
    QMutex m_active_mutex;
    QList<paTestData*> m_active_sounds;

    bool m_virtual_mic_loaded = true;
    bool m_initial_startup = true;
};



int main(int argc, char *argv[])
{
    PaError err;
    QProcess process;
    SQLDatabase db;


    err = Pa_Initialize();
    checkErr(err);
    PaStream* keepAliveStream;
    Pa_OpenDefaultStream(&keepAliveStream, 0, 2, paFloat32, 44100, 512, nullptr, nullptr);
    Pa_StartStream(keepAliveStream);

    QGuiApplication app(argc, argv);
    qmlRegisterSingletonType<Backend>("PARPUI", 1, 0, "Backend",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new Backend();
        });
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("PARPUI", "Main");
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sounds.db";
    if(!QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))) {
        db.Database_create();
    }

    Backend* backend = engine.singletonInstance<Backend*>("PARPUI", "Backend");
    if (backend) {
        backend->load_sounds();
        backend->load_unload_devices();
    }


    int result = QCoreApplication::exec();

    QDir tempdir(QCoreApplication::applicationDirPath() + "/sounds/temp/");
    QStringList files = tempdir.entryList(QStringList() << "*.raw", QDir::Files);
    for (const QString &file : files) {
        tempdir.remove(file);
    }

    err = Pa_Terminate();
    checkErr(err);
    return result;
}

void Backend::load_sounds(){
    m_sounds_path = QCoreApplication::applicationDirPath() + "/sounds";
    SQLDatabase db;
    db.Database_read();
    QDir dir(m_sounds_path);
    if(!dir.exists()){
        QDir appdir(QCoreApplication::applicationDirPath());
        appdir.mkdir("sounds", std::nullopt);
        appdir.mkdir("sounds/temp", std::nullopt);
    }
    QStringList files = dir.entryList(QStringList() << "*.raw" << "*.mp3", QDir::Files);
    m_sounds.clear();
    for(const QString &file : files){
        m_sounds.insert(file, {m_sounds_path + "/" + file, ""});
    }
    emit soundsChanged();
}

void Backend::load_unload_devices(){
    QtConcurrent::run([=](){
    QProcess process;
    if(m_initial_startup){
        process.start("bash", QStringList() << QString(PARP_SOURCE_DIR) + "/unloaddevices");
        m_virtual_mic_button_text = "Load Virtual Mic";
        m_virtual_mic_loaded = false;
        m_initial_startup = false;
        emit virtualmicToggle();
    }else if(!m_virtual_mic_loaded){
        process.start("bash", QStringList() << QString(PARP_SOURCE_DIR) + "/loaddevices");
        m_virtual_mic_button_text = "Unload Virtual Mic";
        m_virtual_mic_loaded = true;
    }else{
        process.start("bash", QStringList() << QString(PARP_SOURCE_DIR) + "/unloaddevices");
        m_virtual_mic_button_text = "Load Virtual Mic";
        m_virtual_mic_loaded = false;
    }
    process.waitForFinished();
    emit virtualmicToggle();
    qDebug() << process.readAllStandardOutput();
    });
}

void Backend::remove_sound(QString file_path){
    Sound *s = find_sound(file_path);
    if(!s) return;

    QDir(m_sounds_path).remove(QFileInfo(file_path).fileName());

    m_sounds.remove(QFileInfo(file_path).fileName());
    emit soundsChanged();
}

static int valid_file(regex_t *regex, char* file_name){
    int ret;
    ret = regexec(regex, file_name, 0, NULL, 0);
    if(!ret)
        return ret;
    else if(ret == REG_NOMATCH){}
    else{
        fprintf(stderr, "REGEX ERROR OCCURRED\n");
        exit(1);
    }
    return ret;
}



void Backend::stop_all(){
    qDebug() << "Stopping all";
    QMutexLocker lock(&m_active_mutex);
    qDebug() << "Acquired stop lock";
    for (paTestData *d : m_active_sounds){
        d->stopRequested = true;
    }
}
void Backend::open_sounds_folder(){
    QDesktopServices::openUrl(QUrl(m_sounds_path));
}
void Backend::add_sound(QString file_path){
    QString cleaned = QUrl(file_path).toLocalFile();
    QString file_name = QFileInfo(cleaned).fileName();
    QString dest_path = m_sounds_path + "/" + file_name;

    qDebug() << "Source:" << cleaned;
    qDebug() << "Destination:" << dest_path;
    qDebug() << "Source exists:" << QFile::exists(cleaned);
    qDebug() << "Sounds dir exists:" << QDir(m_sounds_path).exists();
    if(QFile::exists(dest_path)){
        qDebug() << "File already exists at destination";
        QFile::remove(dest_path);
    }

    //dest_path = QString::fromLatin1(c_dest_path);
    QFile src(cleaned);
    if(src.copy(dest_path)){
        m_sounds.insert(file_name, {dest_path, ""});
        emit soundsChanged();
    } else {
        qDebug() << "Failed:" << src.errorString();
    }

}


void Backend::play(QString file_name){
    QtConcurrent::run([=](){
    char play_target[MAX_FILE_NAME] = {0};

    QByteArray ba = file_name.toLocal8Bit();
    char* c_file_name = ba.data();
    regex_t rawregex, mp3regex;
    regcomp(&mp3regex, "^.+\\.(mp3)$", REG_EXTENDED);
    regcomp(&rawregex, "^.+\\.(raw)$", REG_EXTENDED);

    if(valid_file(&mp3regex, c_file_name) == 0){
        Sound *s = find_sound(file_name);
        if(s && s->is_converted()){
            strncpy(play_target, s->playback_path.toLocal8Bit().data(), MAX_FILE_NAME - 1);
        } else{
            QString file = QFileInfo(file_name).fileName();
            QString mp3_temp = m_sounds_path + "/temp/" + file;
            char raw_name[MAX_FILE_NAME] = {0};

            QFile(file_name).copy(mp3_temp);
            QByteArray tmp_ba = mp3_temp.toLocal8Bit();
            convert_mp3_to_raw(tmp_ba.data(), raw_name, sizeof(raw_name));

            QFile::remove(mp3_temp);

            strncpy(play_target, raw_name, MAX_FILE_NAME - 1);

            if (s) s->playback_path = QString::fromLocal8Bit(raw_name);
        }
    } else if (valid_file(&rawregex, c_file_name) == 0){
        strncpy(play_target, ba.data(), MAX_FILE_NAME - 1);
    }

    regfree(&mp3regex);
    regfree(&rawregex);

    if (play_target[0] == '\0') {
        qDebug() << "play: unrecognised file type, aborting";
        return;
    }

    PaStreamParameters outputParameters;
    PaError err;
    Sound *sound = find_sound(file_name);
    paTestData data = {0};
    data.stopRequested = false;
    data.volume = this->m_volume * (sound ? sound->gain : 1.0f);
    memcpy(data.file_name, play_target, MAX_FILE_NAME);
    unsigned numSamples;
    unsigned numBytes;
    numSamples = NextPowerOf2((unsigned)(SAMPLE_RATE * 0.5 * NUM_CHANNELS));
    numBytes = numSamples * sizeof(SAMPLE);
    data.ringBufferData = (SAMPLE *)PaUtil_AllocateMemory(numBytes);
    if (data.ringBufferData == NULL) {
        printf("Could not allocate ring buffer data.\n");
        exit(1);
    }
    err = PaUtil_InitializeRingBuffer(&data.ringBuffer, sizeof(SAMPLE),
                                      numSamples, data.ringBufferData);
    checkErr(err);


    // playback
    memset(&outputParameters, 0, sizeof(outputParameters));
    outputParameters.channelCount = 2;
    outputParameters.device = Pa_GetDefaultOutputDevice();

    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency =
        Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
    qDebug() << "Registering sound";
    register_sound(&data);
    qDebug() << "Done registering sound";
    PlaySound(outputParameters, &data, err);
    qDebug() << "Unregistering sound";
    unregister_sound(&data);
    qDebug() << "Done unregistering sound";
    if (data.ringBufferData)
        PaUtil_FreeMemory(data.ringBufferData);
    printf("\n");
    });
}
#include "main.moc"