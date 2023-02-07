import CheckPackages
CheckPackages.CheckAndInstallAll({"requests", "fake-useragent"})

import subprocess
import os
import Utility
from operator import contains

EngineDir = ".."

# Check for Vulkan SDK
# Download Vulkan SDK Installer
# Execute Vulkan SDK Installer and wait for completion


def CheckAndDownload(DownloadID, DownloadDestination, ExtractDestination, Title):
    Download = True
    if os.path.exists(os.path.splitext(DownloadDestination)[0]):
        print("'" + Title + "' already exists, would you like to download it again? ", end='')
        Download = Utility.YesOrNo()
    
    if Download:
        Utility.DownloadGoogleDriveAndExtract(DownloadID, DownloadDestination, ExtractDestination, Title)


# Download and extract Thirdparty Libraries if they don't exist
CheckAndDownload("1UlUFGfBAHQwKzsLv5W4SjWpekcIk4gyo", EngineDir + "/ThirdParty.zip", EngineDir, "Thirdparty Libraries")


# Download and extract Engine Resources if they don't exist
#CheckAndDownload("", EngineDir + "/CardinalEngine - Resources.zip", EngineDir, "Engine Resources")


# Download and extract Editor Resources if they don't exist
CheckAndDownload("12PW_zSReyCeOKyWnlCPfPJQpbOL-5l8F", EngineDir + "/CardinalEngine-Editor - Resources.zip", EngineDir, "Editor Resources")


# Select Premake Configuration
AvailablePremakeConfigurations = ["vs2019", "vs2022"]
PremakeConfiguration = ""
print("Enter Premake Configuration ('vs2019', 'vs2022')")
while not contains(AvailablePremakeConfigurations, PremakeConfiguration):
    PremakeConfiguration = input()


# Run Premake
print("Running premake...")
subprocess.call(["premake5.exe", PremakeConfiguration])
