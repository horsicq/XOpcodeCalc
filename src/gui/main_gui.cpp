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

#include "guimainwindow.h"

namespace {

QString versionText()
{
    return QStringLiteral("%1 v%2").arg(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION);
}

bool shouldPrintVersionAndExit(const int argc, char *argv[])
{
    return (argc == 2) && ((QString::fromUtf8(argv[1]) == QStringLiteral("--version")) || (QString::fromUtf8(argv[1]) == QStringLiteral("-v")));
}

void configureApplicationMetadata()
{
    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setOrganizationDomain(X_ORGANIZATIONDOMAIN);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(X_APPLICATIONVERSION);
}

void configureMacPluginPath(char *argv[])
{
#ifdef Q_OS_MAC
#ifndef QT_DEBUG
    QString libraryPath = QString::fromUtf8(argv[0]);
    libraryPath = libraryPath.remove(QStringLiteral("MacOS/XOcalc")) + QStringLiteral("PlugIns");
    QCoreApplication::setLibraryPaths(QStringList(libraryPath));
#else
    Q_UNUSED(argv)
#endif
#else
    Q_UNUSED(argv)
#endif
}

}  // namespace

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    configureMacPluginPath(argv);
    configureApplicationMetadata();

    if (shouldPrintVersionAndExit(argc, argv)) {
        std::puts(versionText().toUtf8().constData());

        return 0;
    }

    QApplication application(argc, argv);
    application.setWindowIcon(QIcon(QStringLiteral(":/images/main.png")));

#ifdef Q_OS_LINUX
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    application.setDesktopFileName(QStringLiteral("xocalc"));
#endif
#endif

    XOptions options;
    options.setName(X_OPTIONSFILE);
    options.addID(XOptions::ID_VIEW_STYLE, QStringLiteral("Fusion"));
    options.load();

    XOptions::adjustApplicationView(X_APPLICATIONNAME, &options);

    GuiMainWindow window;
    window.show();

    return application.exec();
}
