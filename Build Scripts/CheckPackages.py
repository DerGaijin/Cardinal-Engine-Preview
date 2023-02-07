import subprocess
import pkg_resources

def InstallPackage(Package):
    print(f"Installing {Package} module...")
    subprocess.check_call(['python', '-m', 'pip', 'install', Package])

def HasPackageInstalled(Package):
    required = { Package }
    installed = {pkg.key for pkg in pkg_resources.working_set}
    return not (required - installed)

def CheckAndInstallPackage(Package):
    if not HasPackageInstalled(Package):
        print(f"Python is missing package '{Package}' would you like to install it?")
        Install = False
        while True:
            reply = str(input('[Y/N]: ')).lower().strip()
            if reply[:1] == 'y':
                Install = True
                break
            if reply[:1] == 'n':
                exit()
        if Install:
            InstallPackage(Package)

def CheckAndInstallAll(PackageNames):
    for Package in PackageNames:
        CheckAndInstallPackage(Package)
