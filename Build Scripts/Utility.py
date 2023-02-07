import requests
import sys
import time
import os
from zipfile import ZipFile
from fake_useragent import UserAgent

def DownloadResponse(Response, Destination):
    total = Response.headers.get('content-length')
    with open(Destination, "wb") as f:
        if total is None:
            f.write(Response.content)
        else:
            downloaded = 0
            total = int(total)
            startTime = time.time()
            for data in Response.iter_content(chunk_size=max(int(total/1000), 1024*1024)):
                if data:  # filter out keep-alive new chunks
                    downloaded += len(data)
                    f.write(data)
                    done = int(50*downloaded/total)
                    percentage = (downloaded / total) * 100
                    elapsedTime = time.time() - startTime
                    avgKBPerSecond = (downloaded / 1024) / elapsedTime
                    avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
                    if (avgKBPerSecond > 1024):
                        avgMBPerSecond = avgKBPerSecond / 1024
                        avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)
                    sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
                    sys.stdout.flush()
            sys.stdout.write('\n')


def DownloadFile(URL, Destination):
    ua = UserAgent()
    headers = {'User-Agent': ua.chrome}
    response = requests.get(URL, headers=headers, stream=True)
    DownloadResponse(response, Destination)


def DownloadFileFromGoogleDrive(ID, Destination):
    URL = "https://docs.google.com/uc?export=download"

    session = requests.Session()

    params = {'id': ID, 'confirm': "True"}
    response = session.get(URL, params=params, stream=True)

    DownloadResponse(response, Destination)


def ExtractZipFile(ZipPath, ToPath):
    zf = ZipFile(ZipPath)
    zf.extractall(path=ToPath)
    zf.close()


def DownloadGoogleDriveAndExtract(ID, DownloadDestination, ExtractDestination, Title):
    print("Downloading " + Title + "...")
    DownloadFileFromGoogleDrive(ID, DownloadDestination)
    print("Extracting " + Title + "...")
    ExtractZipFile(DownloadDestination, ExtractDestination)
    os.remove(DownloadDestination)


def YesOrNo():
    while True:
        reply = str(input('[Y/N]: ')).lower().strip()
        if reply[:1] == 'y':
            return True
        if reply[:1] == 'n':
            return False
