/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef MACBUNDLEPATH_H
#define MACBUNDLEPATH_H

#include <QString>

// Where a macOS app bundle keeps its Qt plugins, derived from argv[0].
//
// This has to run before QApplication exists — the platform plugin is loaded
// during construction, so QCoreApplication::applicationDirPath() is not an
// option — which is why it works on the raw argv[0] string.
//
// It lives in its own header, compiled on every platform, so that a mistake in
// it is caught by any build rather than only by a macOS release build. The
// previous version stripped a hard-coded "MacOS/XOcalc" while the binary is
// actually named "xocalc" (OUTPUT_NAME in src/gui/CMakeLists.txt), so the strip
// silently did nothing and every release bundle pointed Qt at
// ".../Contents/MacOS/xocalcPlugIns".
namespace MacBundlePath {

// "<bundle>.app/Contents/MacOS/<exe>" -> "<bundle>.app/Contents/PlugIns".
// Returns an empty string when the path is not a bundle executable, so the
// caller leaves Qt's default library paths alone instead of replacing them with
// a directory that does not exist.
inline QString pluginPathFromExecutable(const QString &sExecutablePath, const QString &sExecutableName)
{
    if (sExecutableName.isEmpty()) {
        return QString();
    }

    const QString sSuffix = QStringLiteral("MacOS/") + sExecutableName;

    if (!sExecutablePath.endsWith(sSuffix)) {
        return QString();
    }

    return sExecutablePath.left(sExecutablePath.length() - sSuffix.length()) + QStringLiteral("PlugIns");
}

}  // namespace MacBundlePath

#endif  // MACBUNDLEPATH_H
