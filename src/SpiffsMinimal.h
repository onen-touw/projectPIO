#pragma once

#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true

namespace SPFmin
{
    bool listDir(fs::FS &fs, const char *dirname, uint8_t levels)
    {
        Serial.printf("Listing directory: %s\r\n", dirname);

        File root = fs.open(dirname);
        if (!root)
        {
            Serial.println("- failed to open directory");
            return false;
        }
        if (!root.isDirectory())
        {
            Serial.println(" - not a directory");
            return false;
        }

        File file = root.openNextFile();
        while (file)
        {
            if (file.isDirectory())
            {
                Serial.print("  DIR : ");
                Serial.println(file.name());
                if (levels)
                {
                    listDir(fs, file.path(), levels - 1);
                }
            }
            else
            {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("\tSIZE: ");
                Serial.println(file.size());
            }
            file = root.openNextFile();
        }
        return true;
    }

    bool readFile(fs::FS &fs, const char *path)
    {
        Serial.printf("Reading file: %s\r\n", path);

        File file = fs.open(path);
        if (!file || file.isDirectory())
        {
            Serial.println("- failed to open file for reading");
            return false;
        }

        Serial.println("- read from file:");
        while (file.available())
        {
            Serial.write(file.read());
        }
        file.close();
        return true;
    }

    bool writeFile(fs::FS &fs, const char *path, const char *message)
    {
        Serial.printf("Writing file: %s\r\n", path);
        bool status = true;
        File file = fs.open(path, FILE_WRITE);
        if (!file)
        {
            Serial.println("- failed to open file for writing");
            return false;
        }
        if (file.print(message))
        {
            Serial.println("- file written");
        }
        else
        {
            Serial.println("- write failed");
            status = false;
        }
        file.close();
        return status;
    }

    bool TestReadWrite(fs::FS &fs){
        bool status = writeFile(fs, "/testRW.txt", "TEST MESSAGE\n");
        status = readFile(fs, "/testRW.txt");
        return status;
    }

///
    void appendFile(fs::FS &fs, const char *path, const char *message)
    {
        Serial.printf("Appending to file: %s\r\n", path);

        File file = fs.open(path, FILE_APPEND);
        if (!file)
        {
            Serial.println("- failed to open file for appending");
            return;
        }
        if (file.print(message))
        {
            Serial.println("- message appended");
        }
        else
        {
            Serial.println("- append failed");
        }
        file.close();
    }

    void renameFile(fs::FS &fs, const char *path1, const char *path2)
    {
        Serial.printf("Renaming file %s to %s\r\n", path1, path2);
        if (fs.rename(path1, path2))
        {
            Serial.println("- file renamed");
        }
        else
        {
            Serial.println("- rename failed");
        }
    }

    void deleteFile(fs::FS &fs, const char *path)
    {
        Serial.printf("Deleting file: %s\r\n", path);
        if (fs.remove(path))
        {
            Serial.println("- file deleted");
        }
        else
        {
            Serial.println("- delete failed");
        }
    }
    void testFileIO(fs::FS &fs, const char *path)
    {
        Serial.printf("Testing file I/O with %s\r\n", path);

        static uint8_t buf[512];
        size_t len = 0;
        File file = fs.open(path, FILE_WRITE);
        if (!file)
        {
            Serial.println("- failed to open file for writing");
            return;
        }

        size_t i;
        Serial.print("- writing");
        uint32_t start = millis();
        for (i = 0; i < 2048; i++)
        {
            if ((i & 0x001F) == 0x001F)
            {
                Serial.print(".");
            }
            file.write(buf, 512);
        }
        Serial.println("");
        uint32_t end = millis() - start;
        Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
        file.close();

        file = fs.open(path);
        start = millis();
        end = start;
        i = 0;
        if (file && !file.isDirectory())
        {
            len = file.size();
            size_t flen = len;
            start = millis();
            Serial.print("- reading");
            while (len)
            {
                size_t toRead = len;
                if (toRead > 512)
                {
                    toRead = 512;
                }
                file.read(buf, toRead);
                if ((i++ & 0x001F) == 0x001F)
                {
                    Serial.print(".");
                }
                len -= toRead;
            }
            Serial.println("");
            end = millis() - start;
            Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
            file.close();
        }
        else
        {
            Serial.println("- failed to open file for reading");
        }
    }

}