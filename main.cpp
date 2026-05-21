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
#include <string.h>
#include <regex.h>
#include "parp.h"


class Backend : public QObject{
    Q_OBJECT
    Q_PROPERTY(QStringList sounds READ sounds NOTIFY soundsChanged)
    Q_PROPERTY(QString virtual_mic_button_text READ virtual_mic_button_text  NOTIFY virtualmicToggle)

public:
    Q_INVOKABLE void play(QString file_name);
    Q_INVOKABLE void load_sounds();
    Q_INVOKABLE void add_sound(QString file_path);
    Q_INVOKABLE void load_unload_devices();
    Q_INVOKABLE void remove_sound(QString file_path);
    QStringList sounds() const { return m_sounds; }
    QString virtual_mic_button_text() const {return m_virtual_mic_button_text; }
signals:
    void soundsChanged();
    void virtualmicToggle();

private:
    QStringList m_sounds;
    QString m_virtual_mic_button_text;
    QString m_sounds_path;

    bool m_virtual_mic_loaded = true;
    bool m_initial_startup = true;
};



int main(int argc, char *argv[])
{
    PaError err;
    QProcess process;
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
    Backend* backend = engine.singletonInstance<Backend*>("PARPUI", "Backend");
    if(backend){
        backend->load_sounds();
        backend->load_unload_devices();
    }


    int result = QCoreApplication::exec();

    QDir dir(QCoreApplication::applicationDirPath() + "/sounds/temp/");
    QStringList files = dir.entryList(QStringList() << "*.raw", QDir::Files);
    for (const QString &file : files) {
        dir.remove(file);
    }
    err = Pa_Terminate();
    checkErr(err);
    return result;
}

void Backend::load_sounds(){
    m_sounds_path = QCoreApplication::applicationDirPath() + "/sounds";
    QDir dir(m_sounds_path);
    if(!dir.exists()){
        QDir appdir(QCoreApplication::applicationDirPath());
        appdir.mkdir("sounds", std::nullopt);
        appdir.mkdir("sounds/temp", std::nullopt);
    }
    QStringList files = dir.entryList(QStringList() << "*.raw" << "*.mp3", QDir::Files);
    m_sounds.clear();
    for(const QString &file : files){
        m_sounds.append(m_sounds_path + "/" + file);
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

void Backend::remove_sound(QString file_name){
    QProcess process;
    process.start("rm", QStringList() << "-f" << file_name);
    process.waitForFinished();
    m_sounds.removeAll(file_name);
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

void Backend::add_sound(QString file_path){
    QString cleaned = QUrl(file_path).toLocalFile();
    QString file_name = QFileInfo(file_path).fileName();
    QString dest_path = m_sounds_path + "/" + file_name;

    qDebug() << "Source:" << cleaned;
    qDebug() << "Destination:" << dest_path;
    qDebug() << "Source exists:" << QFile::exists(cleaned);
    qDebug() << "Sounds dir exists:" << QDir(m_sounds_path).exists();
    if(QFile::exists(dest_path)){
        qDebug() << "File already exists at destination";
        // add replace if it exists prompt
        QFile::remove(dest_path);
    }

    //dest_path = QString::fromLatin1(c_dest_path);
    QFile src(cleaned);
    if(src.copy(dest_path)){
        m_sounds.append(dest_path);
        emit soundsChanged();
    } else {
        qDebug() << "Failed:" << src.errorString();
    }

}

void Backend::play(QString file_name){
    QtConcurrent::run([=](){
    QByteArray ba = file_name.toLocal8Bit();
    char* c_file_name = ba.data();
    regex_t mp3regex;
    regex_t rawregex;

    regcomp(&mp3regex, "^.+\\.(mp3)$", REG_EXTENDED);
    regcomp(&rawregex, "^.+\\.(raw)$", REG_EXTENDED);
    if(valid_file(&mp3regex, c_file_name) == 0){
        char raw_name[MAX_FILE_NAME] = {0};
        QFile src(file_name);
        QString file = file_name.split('/').last();
        QString mp3_temp_path = m_sounds_path + "/temp/" + file;
        QByteArray ba = mp3_temp_path.toLocal8Bit();
        char* c_mp3_temp_name = ba.data();
        src.copy(mp3_temp_path);
        convert_mp3_to_raw(c_mp3_temp_name, raw_name, sizeof(raw_name));
        snprintf(c_file_name, sizeof(raw_name), "%s", raw_name);
        QDir dir(m_sounds_path + "/temp/");
        QStringList files = dir.entryList(QStringList() << "*.mp3", QDir::Files);
        for (const QString &file : files) {
            dir.remove(file);
        }
        m_sounds.removeAll(file_name);
        m_sounds.append(QString::fromLatin1(c_file_name));
        emit soundsChanged();
    }
    PaStreamParameters outputParameters;
    PaError err;

    paTestData data = {0};
    memcpy(data.file_name, c_file_name, MAX_FILE_NAME);
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
    PlaySound(outputParameters, &data, err);

    if (data.ringBufferData)
        PaUtil_FreeMemory(data.ringBufferData);
    printf("\n");
    });
}
#include "main.moc"