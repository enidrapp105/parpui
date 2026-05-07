#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <string.h>
#include <QObject>
#include <QQmlEngine>
#include "parp.h"

class Backend : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE void play(QString file_name);
};

int main(int argc, char *argv[])
{
    PaError err;
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

    int result = QCoreApplication::exec();
    err = Pa_Terminate();
    checkErr(err);
    return result;
}


void Backend::play(QString file_name){
    QByteArray ba = file_name.toLocal8Bit();
    char* c_file_name = ba.data();

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

}
#include "main.moc"