# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Tested with QT 5.15.2 on Windows 10
# https://mrfaptastic.github.io

<#
 .SYNOPSIS

  Build a static version of Qt for Windows.

 .DESCRIPTION

  This scripts downloads Qt source code, compiles and installs a static version
  of Qt. It assumes that a prebuilt Qt / MinGW environment is already installed,
  typically in C:\Qt. This prebuilt environment uses shared libraries. It is
  supposed to remain the main development environment for Qt. This script adds
  a static version of the Qt libraries in order to allow the construction of
  standalone and self-sufficient executable.

  This script is typically run from the Windows Explorer.

  Requirements:
  - Windows PowerShell 3.0 or higher.
  - 7-zip.

 .PARAMETER QtSrcUrl

  URL of the Qt source file archive.
  By default, use the latest identified version.

 .PARAMETER QtStaticDir

  Root directory where the static versions of Qt are installed.
  By default, use C:\Qt\Static.

 .PARAMETER QtVersion

  The Qt version. By default, this script tries to extract the version number
  from the Qt source file name.

 .PARAMETER MingwDir

  Root directory of the MinGW prebuilt environment. By default, use the version
  which was installed by the prebuilt Qt environment.

 .PARAMETER NoPause

  Do not wait for the user to press <enter> at end of execution. By default,
  execute a "pause" instruction at the end of execution, which is useful
  when the script was run from Windows Explorer.
#>

[CmdletBinding()]
param(
    $QtSrcUrl = "https://download.qt.io/official_releases/qt/5.15/5.15.2/single/qt-everywhere-src-5.15.2.zip",
    $QtStaticDir = "C:\Qt\Static", # NO TRAILING SLASH
    $QtVersion = "5.15.2", #If you change this, you'll need to change the URL above to download as well...
    $MingwDir = "C:\Qt\Tools\mingw810_32",
    [switch]$NoPause = $false
)

# PowerShell execution policy.
Set-StrictMode -Version 3

#-----------------------------------------------------------------------------
# Main code
#-----------------------------------------------------------------------------

function Main
{
    # Check that 7zip is installed. We use it to expand the downloaded archive.
    [void] (Get-7zip)
	
    # Get Qt source file name from URL.
    $QtSrcFileName = Split-Path -Leaf $QtSrcUrl

    # If Qt version is not specified on the command line, try to extract the value.
    if (-not $QtVersion) {
        $QtVersion = $QtSrcFileName -replace "`.[^`.]*$",''
        $QtVersion = $QtVersion -replace 'qt-',''
        $QtVersion = $QtVersion -replace 'everywhere-',''
        $QtVersion = $QtVersion -replace 'opensource-',''
        $QtVersion = $QtVersion -replace 'src-',''
        $QtVersion = $QtVersion -replace '-src',''
    }
    Write-Output "Building static Qt version $QtVersion"

    # Qt installation directory.
    $QtDir = "$QtStaticDir\$QtVersion"

    # Get MinGW root directory, if not specified on the command line.
    if (-not $MingwDir) {
        # Search all instances of gcc.exe from C:\Qt prebuilt environment.
        $GccList = @(Get-ChildItem -Path C:\Qt\Tools\mingw*\bin\gcc.exe | ForEach-Object FullName | Sort-Object)
        if ($GccList.Length -eq 0) {
            Exit-Script "MinGW environment not found, no Qt prebuilt version?"
        }
        $MingwDir = (Split-Path -Parent (Split-Path -Parent $GccList[$GccList.Length - 1]))
    }
    Write-Output "Using MinGW from $MingwDir"

    # Build the directory tree where the static version of Qt will be installed.
    Create-Directory $QtStaticDir\src
    Create-Directory $QtDir

    # Download the Qt source package if not yet done.
    Download-File $QtSrcUrl $QtStaticDir\src\$QtSrcFileName

    # Directory of expanded packages.
    $QtSrcDir = "$QtStaticDir\src\$((Get-Item $QtStaticDir\src\$QtSrcFileName).BaseName)"

    # Expand archives if not yet done
    Expand-Archive $QtStaticDir\src\$QtSrcFileName $QtStaticDir\src $QtSrcDir

    # Patch Qt's mkspecs for static build.
    $File = "$QtSrcDir\qtbase\mkspecs\win32-g++\qmake.conf"
    if (-not (Select-String -Quiet -SimpleMatch -CaseSensitive "# [QT-STATIC-PATCH]" $File)) {
        Write-Output "Patching $File ..."
        Copy-Item $File "$File.orig"
        @"

# [QT-STATIC-PATCH]
QMAKE_LFLAGS += -static -static-libgcc
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -Os -momit-leaf-frame-pointer
DEFINES += QT_STATIC_BUILD
"@ | Out-File -Append $File -Encoding Ascii
    }

    # Set a clean path including MinGW.
    $env:Path += "$MingwDir\bin;$MingwDir\opt\bin;$env:SystemRoot\system32;$env:SystemRoot;$env:SystemRoot\system32\WindowsPowerShell\v1.0\;"
	
	
    # Check that the 'powershell' command is available
	# ... and ruby, python and perl - as per: qt-everywhere-src-X.XX.X\README
	#
	# https://forum.qt.io/topic/118511/static-qt-environment-error-using-qt5-15-0/7
	# 
	[void] (Check-prequisites)	
	

    # Force English locale to avoid weird effects of tools localization.
    $env:LANG = "en"

    # Set environment variable QT_INSTALL_PREFIX. Documentation says it should be
    # used by configure as prefix but this does not seem to work. So, we will
    # also specify -prefix option in configure.
    $env:QT_INSTALL_PREFIX = $QtDir

    # Configure, compile and install Qt.
    Push-Location $QtSrcDir
    cmd /c "configure.bat -static -release -platform win32-g++ -opensource -confirm-license -prefix $QtDir -qt-zlib -qt-libpng -qt-webp -qt-libjpeg -qt-freetype  -no-opengl -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdeclarative -skip qtdoc -skip qtgamepad -skip qtlocation -skip qtlottie -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquick3d -skip qtquickcontrols -skip qtquickcontrols2 -skip qtquicktimeline -skip qtremoteobjects -skip qtsensors -skip qtspeech -skip qtsvg -skip qtwayland -skip qtwebglplugin -skip qtwebview -skip webengine -make libs -nomake tools -nomake examples -nomake tests"
    mingw32-make #-k -j4
    mingw32-make install #-k install
    Pop-Location

    # Patch Qt's installed mkspecs for static build of application.
    $File = "$QtDir\mkspecs\win32-g++\qmake.conf"
    @"
CONFIG += static
"@ | Out-File -Append $File -Encoding Ascii

    Exit-Script
}

#-----------------------------------------------------------------------------
# A function to exit this script. The Message parameter is used on error.
#-----------------------------------------------------------------------------

function Exit-Script ([string]$Message = "")
{
    $Code = 0
    if ($Message -ne "") {
        Write-Output "ERROR: $Message"
        $Code = 1
    }
    if (-not $NoPause) {
        pause
    }
    exit $Code
}

#-----------------------------------------------------------------------------
# Silently create a directory.
#-----------------------------------------------------------------------------

function Create-Directory ([string]$Directory)
{
    [void] (New-Item -Path $Directory -ItemType "directory" -Force)
}

#-----------------------------------------------------------------------------
# Download a file if not yet present.
# Warning: If file is present but incomplete, do not download it again.
#-----------------------------------------------------------------------------

function Download-File ([string]$Url, [string]$OutputFile)
{
    $FileName = Split-Path $Url -Leaf
    if (-not (Test-Path $OutputFile)) {
        # Local file not present, start download.
        Write-Output "Downloading $Url ..."
        try {
            $webclient = New-Object System.Net.WebClient
            $webclient.DownloadFile($Url, $OutputFile)
        }
        catch {
            # Display exception.
            $_
            # Delete partial file, if any.
            if (Test-Path $OutputFile) {
                Remove-Item -Force $OutputFile
            }
            # Abort
            Exit-Script "Error downloading $FileName"
        }
        # Check that the file is present.
        if (-not (Test-Path $OutputFile)) {
            Exit-Script "Error downloading $FileName"
        }
    }
}

#-----------------------------------------------------------------------------
# Get path name of 7zip, abort if not found.
#-----------------------------------------------------------------------------

function Get-7zip
{
    $Exe = "C:\Program Files\7-Zip\7z.exe"
    if (-not (Test-Path $Exe)) {
        $Exe = "C:\Program Files (x86)\7-Zip\7z.exe"
    }
    if (-not (Test-Path $Exe)) {
        Exit-Script "7-zip not found, install it first, see http://www.7-zip.org/"
    }
    $Exe
}

#-----------------------------------------------------------------------------
# Check if PowerShell is available.
#-----------------------------------------------------------------------------

function Check-prequisites
{
	if (Get-Command "powershell.exe" -ErrorAction SilentlyContinue) 
	{ 
	   echo "PowerShell is available.. This is good"
	}
	else
	{
       Exit-Script "'PowerShell' command is not available, check your windows path variable for this user account."
	}	

	if (Get-Command "ruby" -ErrorAction SilentlyContinue) 
	{ 
	   echo "Ruby is available.. This is good"
	}
	else
	{
       Exit-Script "'ruby' is not available, check your windows path variable for this user account or install per the Qt everything instructions!"
	}	 	
	

	if (Get-Command "python" -ErrorAction SilentlyContinue) 
	{ 
	   echo "Python is available.. This is good"
	}
	else
	{
       Exit-Script "'python' is not available, check your windows path variable for this user account or install ActivePython per the Qt everything instructions!"
	}	 	

	if (Get-Command "perl" -ErrorAction SilentlyContinue) 
	{ 
	   echo "perl is available.. This is good"
	}
	else
	{
       Exit-Script "'perl' is not available, check your windows path variable for this user account or install ActivePerl per the Qt everything instructions!"
	}	 		
	

}


#-----------------------------------------------------------------------------
# Expand an archive file if not yet done.
#-----------------------------------------------------------------------------

function Expand-Archive ([string]$ZipFile, [string]$OutDir, [string]$CheckFile)
{
    # Check presence of expected expanded file or directory.
    if (-not (Test-Path $CheckFile)) {
        Write-Output "Expanding $ZipFile ..."
        & (Get-7zip) x $ZipFile "-o$OutDir" | Select-String -Pattern "^Extracting " -CaseSensitive -NotMatch
        if (-not (Test-Path $CheckFile)) {
            Exit-Script "Error expanding $ZipFile, $OutDir\$CheckFile not found"
        }
    }
}

#-----------------------------------------------------------------------------
# Execute main code.
#-----------------------------------------------------------------------------

. Main
