/***********************************
 * Project: PARPUI
 * File:    backend.cpp
 * Author:  Enid Rapp
 */
#include "backend.h"

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

void Backend::play(QString file_name){
    (void)QtConcurrent::run([=](){
        char play_target[MAX_FILE_NAME] = {0};
        SQLDatabase *db;
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
                QString mp3_temp = m_sounds_path + "/temp/" + file_name;
                char raw_name[MAX_FILE_NAME] = {0};

                QFile(s->display_path).copy(mp3_temp);
                QByteArray tmp_ba = mp3_temp.toLocal8Bit();
                convert_mp3_to_raw(tmp_ba.data(), raw_name, sizeof(raw_name));

                QFile::remove(mp3_temp);

                strncpy(play_target, raw_name, MAX_FILE_NAME - 1);

                if (s) s->playback_path = QString::fromLocal8Bit(raw_name);
            }
        } else if (valid_file(&rawregex, c_file_name) == 0){
            Sound *s = find_sound(file_name);
            strncpy(play_target, s->display_path.toLocal8Bit().data(), MAX_FILE_NAME - 1);
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
        data.volume = this->m_settings.volume * (sound ? sound->gain : 1.0f);
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
        //db->Database_write(sound);
    });
}

void Backend::load_sounds(){
    m_sounds_path = QCoreApplication::applicationDirPath() + "/sounds";
    SQLDatabase db;
    QDir dir(m_sounds_path);
    if(!dir.exists()){
        QDir appdir(QCoreApplication::applicationDirPath());
        appdir.mkdir("sounds", std::nullopt);
        appdir.mkdir("sounds/temp", std::nullopt);
    }
    QStringList files = dir.entryList(QStringList() << "*.raw" << "*.mp3", QDir::Files);
    m_sounds.clear();
    m_sounds = db.Database_soundinfo_read();
    emit soundsChanged();
}

void Backend::add_sound(QString file_path){
    QString cleaned = QUrl(file_path).toLocalFile();
    QString file_name = QFileInfo(cleaned).fileName();
    QString dest_path = m_sounds_path + "/" + file_name;
    SQLDatabase *db;

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
        Sound sound = {
            .display_path = dest_path,
            .sound_name = file_name

        };
        sound.sound_id = db->Database_soundinfo_write(&sound);
        m_sounds.insert(file_name, sound);

        emit soundsChanged();
    } else {
        qDebug() << "Failed:" << src.errorString();
    }

}

void Backend::load_unload_devices(){
    (void)QtConcurrent::run([=](){
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
    //sound_id isnt populated so the remove row query wont work
    SQLDatabase db;
    if(!s) return;

    QDir(m_sounds_path).remove(s->display_path);
    if(s->is_converted())
        QDir(m_sounds_path).remove(s->playback_path);

    db.Database_soundinfo_remove_row(s);
    m_sounds.remove(s->sound_name);

    emit soundsChanged();
}

void Backend::stop_all(){
    qDebug() << "Stopping all";
    QMutexLocker lock(&m_active_mutex);
    qDebug() << "Acquired stop lock";
    for (paTestData *d : std::as_const(m_active_sounds)){
        d->stopRequested = true;
    }
}

void Backend::open_sounds_folder(){
    QDesktopServices::openUrl(QUrl(m_sounds_path));
}

