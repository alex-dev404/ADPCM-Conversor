#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <QMessageBox>
struct AdpcmHeader
{
    char id[4];
    short version;
    char channels;
    char loop;
    short reserved;
    short pitch;
    int samples;
};


#define VERSION "1.2"
#define BUFFER_SIZE 128 * 28

static short samples[BUFFER_SIZE];
static char wave[BUFFER_SIZE * 2];

static void find_predict(short *samples, double *d_samples, int *predict_nr, int *shift_factor);
static void pack(const double *d_samples, short *four_bit, int predict_nr, int shift_factor);

int adpcm_encode(FILE *fp, FILE *sad, int offset, int sample_len, int flag_loop, int bytes_per_sample)
{
    short *ptr;
    double d_samples[28];
    short four_bit[28];
    int predict_nr;
    int shift_factor;
    int flags;
    int i, j, k;
    unsigned char d;

    flags = 0;
    shift_factor = 0;
    predict_nr = 0;

    while (sample_len > 0)
    {
        int size;

        size = (sample_len >= BUFFER_SIZE) ? BUFFER_SIZE : sample_len;

        if (offset)
        {
            for (i = 0; i < size; i++)
            {
                if (fread(wave + i, bytes_per_sample, 1, fp) == 1)
                {
                    fseek(fp, offset, SEEK_CUR);
                }
                else
                {
                    printf("Error: Can't read SAMPLE DATA in WAVE-file.\n");
                    return EIO;
                }
            }
        }
        else
        {
            if (fread(wave, bytes_per_sample, size, fp) != size)
            {
                printf("Error: Can't read SAMPLE DATA in WAVE-file.\n");
                return EIO;
            }
        }

        if (bytes_per_sample == 1)
        {
            for (i = 0; i < size; i++)
            {
                samples[i] = wave[i];
                samples[i] ^= 0x80;
                samples[i] <<= 8;
            }
        }
        else
        {
            memcpy(samples, wave, BUFFER_SIZE * 2);
        }

        i = size / 28;

        if (size % 28)
        {
            for (j = size % 28; j < 28; j++)
                samples[28 * i + j] = 0;
            i++;
        }

        for (j = 0; j < i; j++)
        {
            ptr = samples + j * 28;

            find_predict(ptr, d_samples, &predict_nr, &shift_factor);

            pack(d_samples, four_bit, predict_nr, shift_factor);

            d = (predict_nr << 4) | shift_factor;
            fputc(d, sad);

            if (flag_loop == 1)
            {
                fputc(6, sad); // loop value
                flag_loop = 2;
            }
            else
            {
                fputc(flags, sad);
            }

            for (k = 0; k < 28; k += 2)
            {
                d = ((four_bit[k + 1] >> 8) & 0xf0) | ((four_bit[k] >> 12) & 0xf);
                fputc(d, sad);
            }

            sample_len -= 28;

            if (sample_len < 28)
            {
                if (flag_loop == 2)
                    flags = 3;
                else
                    flags = 1;
            }
        }
    }

    fputc((predict_nr << 4) | shift_factor, sad);
    fputc(7, sad); // end flag

    for (i = 0; i < 14; i++)
        fputc(0, sad);

    return 0;
}

static double f[5][2] = {{0.0, 0.0},
                         {-60.0 / 64.0, 0.0},
                         {-115.0 / 64.0, 52.0 / 64.0},
                         {-98.0 / 64.0, 55.0 / 64.0},
                         {-122.0 / 64.0, 60.0 / 64.0}};

static void find_predict(short *samples, double *d_samples, int *predict_nr, int *shift_factor)
{
    int i, j;
    double buffer[28][5];
    double min = 1e10;
    double max[5];
    double ds;
    int min2;
    int shift_mask;
    static double _s_1 = 0.0;
    static double _s_2 = 0.0;
    double s_0, s_1, s_2;

    for (i = 0; i < 5; i++)
    {
        max[i] = 0.0;
        s_1 = _s_1;
        s_2 = _s_2;
        for (j = 0; j < 28; j++)
        {
            s_0 = (double)samples[j];
            if (s_0 > 30719.0)
                s_0 = 30719.0;
            if (s_0 < -30720.0)
                s_0 = -30720.0;
            ds = s_0 + s_1 * f[i][0] + s_2 * f[i][1];
            buffer[j][i] = ds;
            if (fabs(ds) > max[i])
                max[i] = fabs(ds);
            s_2 = s_1;
            s_1 = s_0;
        }

        if (max[i] < min)
        {
            min = max[i];
            *predict_nr = i;
        }
        if (min <= 7)
        {
            *predict_nr = 0;
            break;
        }
    }

    _s_1 = s_1;
    _s_2 = s_2;

    for (i = 0; i < 28; i++)
        d_samples[i] = buffer[i][*predict_nr];

    min2 = (int)min;
    shift_mask = 0x4000;
    *shift_factor = 0;

    while (*shift_factor < 12)
    {
        if (shift_mask & (min2 + (shift_mask >> 3)))
            break;
        (*shift_factor)++;
        shift_mask = shift_mask >> 1;
    }
}

static void pack(const double *d_samples, short *four_bit, int predict_nr, int shift_factor)
{
    static double s_1 = 0.0;
    static double s_2 = 0.0;
    int i;

    for (i = 0; i < 28; i++)
    {
        double ds;
        int di, di_shift_tmp;
        double s_0;

        s_0 = d_samples[i] + s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
        ds = s_0 * (double)(1 << shift_factor);

        di = ((int)ds + 0x800) & 0xfffff000;

        if (di > 32767)
            di = 32767;
        if (di < -32768)
            di = -32768;

        four_bit[i] = (short)di;

        di_shift_tmp = -((unsigned int)di >> 31);
        di = (di_shift_tmp ^ di) >> shift_factor ^ di_shift_tmp;
        s_2 = s_1;
        s_1 = (double)di - s_0;
    }
}

int convertToAdpcm(const char *inputFile, const char *outputFile, int flagLoop)
{
    FILE *fp, *sad;
    int sampleFreq, sampleLen, result;
    char s[4];
    int chunkData;
    short e;
    char channels;
    char bytesPerSample;
    struct AdpcmHeader adpcmHeader;

    result = 0;
    if ((fp = fopen(inputFile, "rb")) != NULL)
    {
        if (fread(s, 1, 4, fp) != 4 || strncmp(s, "RIFF", 4))
        {
            printf("Error: Not a WAVE-file (\"RIFF\" identifier not found)\n");
            result = -3;
            goto InputFileIOEnd;
        }

        fseek(fp, 8, SEEK_SET);

        if (fread(s, 1, 4, fp) != 4 || strncmp(s, "WAVE", 4))
        {
            printf("Error: Not a WAVE-file (\"WAVE\" identifier not found)\n");
            result = -3;
            goto InputFileIOEnd;
        }

        fseek(fp, 8 + 4, SEEK_SET);

        if (fread(s, 1, 4, fp) != 4 || strncmp(s, "fmt", 3))
        {
            printf("Error: Not a WAVE-file (\"fmt\" chunk not found)\n");
            result = -3;
            goto InputFileIOEnd;
        }

        if (fread(&chunkData, 4, 1, fp) == 1)
        {
            chunkData += ftell(fp);

            if (fread(&e, 2, 1, fp) != 1 || e != 1)
            {
                printf("Error: No PCM data in WAVE-file\n");
                result = -4;
                goto InputFileIOEnd;
            }
        }
        else
        {
            printf("Error: can't read CHUNK DATA in WAVE-file\n");
            result = -4;
            goto InputFileIOEnd;
        }

        if (fread(&e, 2, 1, fp) != 1 || ((e != 1) && (e != 2)))
        {
            printf("Error: WAVE file must be MONO or STEREO (max 2 channels)\n");
            result = -5;
            goto InputFileIOEnd;
        }

        channels = e;

        if (fread(&sampleFreq, 4, 1, fp) == 1)
        {
            fseek(fp, 4 + 2, SEEK_CUR);
            if (fread(&e, 2, 1, fp) != 1 || (e != 8 && e != 16))
            {
                printf("Error: WAVE-file must 8 or 16 bit\n");
                result = -6;
                goto InputFileIOEnd;
            }
            bytesPerSample = e / 8;
        }
        else
        {
            printf("Error: Can't read SAMPLE FREQUENCY in WAVE-file\n");
            result = -6;
            goto InputFileIOEnd;
        }

        fseek(fp, chunkData, SEEK_SET);
        if (fread(s, 1, 4, fp) == 4)
        {
            // Skip 'fact' and possibly other chunks
            while (strncmp(s, "data", 4))
            {
                if (fread(&chunkData, 4, 1, fp) == 1)
                {
                    chunkData += ftell(fp);
                    fseek(fp, chunkData, SEEK_SET);
                    if (fread(s, 1, 4, fp) != 4)
                    {
                        printf("Error: Read error in WAVE-file\n");
                        result = -6;
                        goto InputFileIOEnd;
                    }
                }
                else
                {
                    printf("Error: Read error in WAVE-file\n");
                    result = -6;
                    goto InputFileIOEnd;
                }
            }
        }
        else
        {
            printf("Error: Read error in WAVE-file\n");
            result = -6;
            goto InputFileIOEnd;
        }

        if (fread(&sampleLen, 4, 1, fp) == 1)
        {
            sampleLen /= (channels * bytesPerSample);
        }
        else
        {
            printf("Error: Can't read SAMPLE LENGTH in WAVE-file\n");
            result = -6;
            goto InputFileIOEnd;
        }

        if ((sad = fopen(outputFile, "wb")) != NULL)
        {
            adpcmHeader = (struct AdpcmHeader){
                .id ={'A', 'P', 'C', 'M'},
                .version = 1,
                .channels = channels,
                .loop = static_cast<char>(flagLoop),
                .reserved = 0,
                .pitch = static_cast<short>((sampleFreq * 4096) / 48000),// pitch, to encode for PS1 change 48000 to 44100
                .samples = sampleLen
            };
            fwrite(&adpcmHeader, sizeof(adpcmHeader), 1, sad);

            if (channels == 1)
            {
                result = adpcm_encode(fp, sad, 0, sampleLen, flagLoop, bytesPerSample);
            }
            else
            {
                int dataOffset = ftell(fp);

                // Encode left
                if ((result = adpcm_encode(fp, sad, bytesPerSample, sampleLen, flagLoop, bytesPerSample)) == 0)
                {
                    fseek(fp, dataOffset + bytesPerSample, SEEK_SET);
                    // Encode right
                    result = adpcm_encode(fp, sad, bytesPerSample, sampleLen, flagLoop, bytesPerSample);
                }
            }

            fclose(sad);
        }
        else
        {
            printf("Error: Can't write output file %s\n", outputFile);
            result = EIO;
        }

    InputFileIOEnd:
        fclose(fp);
    }
    else
    {
        printf("Error: Can't open %s\n", inputFile);
        result = ENOENT;
    }

    return result;
}

int mainWindowMain(int argc, char *argv[])
{
    int result;

    if (argc == 4)
    {
        if (strncmp(argv[1], "-L", 2))
        {
            printf("Error: Option '%s' not recognized\n", argv[1]);
            result = EINVAL;
        }
        else
        {
            result = convertToAdpcm(argv[2], argv[3], 1);
        }
    }
    else if (argc == 3)
    {
        result = convertToAdpcm(argv[1], argv[2], 0);
    }
    else
    {
        printf("ADPCM Encoder %s\n"
               "Usage: sadenc [-L] <input wave> <output sad>\n"
               "Options:\n"
               "  -L  Loop\n",
               VERSION);
        result = EINVAL;
    }

    return result;
}
QString folderName="";
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStringListModel(this))
{
    ui->setupUi(this);

    ui->listView->setModel(model);
    setWindowTitle("WAV_TO_ADP");
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_load_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Selecione um ou mais arquivos"), QDir::homePath());
    model->setStringList(fileNames);
}

void MainWindow::on_btn_output_clicked()
{
    folderName = QFileDialog::getExistingDirectory(this, tr("Selecione uma pasta"), QDir::homePath());
}

void MainWindow::on_btn_converter_clicked()
{
    QString outputFolder = folderName;

    if (outputFolder.isEmpty())
    {
        qDebug() << "Selecione uma pasta de saída!";
        return;
    }

    QStringList fileNames = model->stringList();

    // Percorrer a lista de arquivos
    for (int i = 0; i < fileNames.size(); ++i)
    {
        QFileInfo fileInfo(fileNames[i]);
        QString outputFileName = fileInfo.baseName() + ".adp";
        QString outputPath = outputFolder + "/" + outputFileName;

        int result = convertToAdpcm(fileNames[i].toStdString().c_str(), outputPath.toStdString().c_str(), 0);

        if (result == 0)
        {
            qDebug() << "Conversão bem-sucedida para:" << outputPath;
        }
        else
        {
            qDebug() << "Erro ao converter o arquivo:" << fileNames[i];
        }

        int progressValue = ((i + 1) * 100) / fileNames.size();
        ui->progressBar->setValue(progressValue);
        QCoreApplication::processEvents();
    }
    QMessageBox::information(this, tr("Concluído"), tr("Processo concluído com sucesso!"));
    ui->progressBar->setValue(0);

}

void MainWindow::on_progressBar_valueChanged(int value)
{

}


