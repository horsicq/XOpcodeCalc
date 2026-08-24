/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <QApplication>
#include <QIcon>
#include <cstdio>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "../macbundlepath.h"
#include "guimainwindow.h"

namespace {

// Qt decides the application's layout direction by asking the installed translator
// for QGuiApplication::tr("QT_LAYOUT_DIRECTION") and comparing it with "RTL"
// (qt_detectRTLLanguage() in qguiapplication.cpp). Nothing in this application
// calls that, so lupdate never created the entry and a translator had no way to
// ask for a right-to-left layout - the Arabic, Hebrew and Persian UIs rendered
// right-to-left text inside a left-to-right layout. QT_TRANSLATE_NOOP registers
// the string in that exact context and keeps it across lupdate runs.
const char *const g_pszLayoutDirection = QT_TRANSLATE_NOOP("QGuiApplication", "QT_LAYOUT_DIRECTION");

QString versionText()
{
    return QStringLiteral("%1 v%2").arg(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION);
}

bool shouldPrintVersionAndExit(const int argc, char *argv[])
{
    return (argc == 2) && ((QString::fromUtf8(argv[1]) == QStringLiteral("--version")) || (QString::fromUtf8(argv[1]) == QStringLiteral("-v")));
}

// A GUI-subsystem binary owns no console, so --version used to write into the void
// when it was started from cmd.exe or PowerShell. Borrow the parent's console -
// but only when stdout is not already going somewhere, otherwise this would
// hijack a redirect to a file or a pipe.
void attachParentConsole()
{
#ifdef Q_OS_WIN
    const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if ((hStdOut != nullptr) && (hStdOut != INVALID_HANDLE_VALUE)) {
        return;  // already redirected; leave it alone
    }

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
#if defined(_MSC_VER)
        FILE *pStream = nullptr;
        freopen_s(&pStream, "CONOUT$", "w", stdout);
        freopen_s(&pStream, "CONOUT$", "w", stderr);
#else
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif
        // The shell has already printed its prompt by now, so start on a fresh line.
        std::fputc('\n', stdout);
    }
#endif
}

void configureApplicationMetadata()
{
    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setOrganizationDomain(X_ORGANIZATIONDOMAIN);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(X_APPLICATIONVERSION);
}

// Point Qt at the plugins inside the bundle, and only at those, so a release
// build never picks up a system-wide Qt installation. Must happen before
// QApplication is constructed: that is when the platform plugin is loaded.
void configureMacPluginPath(char *argv[])
{
#if defined(Q_OS_MAC) && !defined(QT_DEBUG)
    const QString sPluginPath = MacBundlePath::pluginPathFromExecutable(QString::fromUtf8(argv[0]), QStringLiteral(X_APPLICATIONNAME));

    if (!sPluginPath.isEmpty()) {
        QCoreApplication::setLibraryPaths(QStringList(sPluginPath));
    }
#else
    Q_UNUSED(argv)
#endif
}

}  // namespace

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // Qt 6 always scales for High-DPI and deprecated the attribute.
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    Q_UNUSED(g_pszLayoutDirection)

    configureMacPluginPath(argv);
    configureApplicationMetadata();

    if (shouldPrintVersionAndExit(argc, argv)) {
        attachParentConsole();
        std::puts(versionText().toUtf8().constData());
        std::fflush(stdout);

        return 0;
    }

    QApplication application(argc, argv);
    application.setWindowIcon(QIcon(QStringLiteral(":/images/logo.png")));

#ifdef Q_OS_LINUX
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    application.setDesktopFileName(QStringLiteral("xocalc"));
#endif
#endif

    XOptions options;
    options.setName(X_OPTIONSFILE);
    options.addID(XOptions::ID_VIEW_STYLE, QStringLiteral("Fusion"));
    // Without this, adjustApplicationView() skips the translator entirely and the
    // 22 .qm files installed under lang/ can never be loaded.
    options.addID(XOptions::ID_VIEW_LANG, QStringLiteral("System"));
    options.load();

    XOptions::adjustApplicationView(X_APPLICATIONNAME, &options);

    GuiMainWindow window;
    window.show();

    return application.exec();
}
