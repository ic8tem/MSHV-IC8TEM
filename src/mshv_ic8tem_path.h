/* MSHV-IC8TEM: writable data folder (settings, log, RxWavs, ...)
 *
 * MSHV keeps all its data next to the executable. That works for a portable
 * Windows folder, but not for read-only installs (macOS .app in /Applications,
 * AppImage, .deb/.rpm in /opt, BSD packages).
 *
 *  - MSHV_DATA_DIR env set  -> data there; seeded (no overwrite) from
 *                              MSHV_SEED_DIR (or the executable folder)
 *  - macOS                  -> ~/Library/Application Support/MSHV-IC8TEM,
 *                              seeded from MSHV.app/Contents/Resources
 *  - otherwise              -> executable folder (original MSHV behaviour) if writable,
 *                              else %APPDATA%\MSHV-IC8TEM or ~/.local/share/MSHV-IC8TEM
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef MSHV_IC8TEM_PATH_H
#define MSHV_IC8TEM_PATH_H

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QByteArray>

inline void MshvCopyTreeNoOverwrite(const QString &src,const QString &dst)
{
    QDir sd(src);
    if (!sd.exists()) return;
    QDir().mkpath(dst);
    QFileInfoList l = sd.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    for (int i = 0; i < l.count(); ++i)
    {
        const QFileInfo &fi = l.at(i);
        QString t = dst+"/"+fi.fileName();
        if (fi.isDir()) MshvCopyTreeNoOverwrite(fi.absoluteFilePath(),t);
        else if (!QFile::exists(t)) QFile::copy(fi.absoluteFilePath(),t);
    }
}
inline QString MshvAppPath()
{
    static QString cached;
    if (!cached.isEmpty()) return cached;
    QString exe_dir = QCoreApplication::applicationDirPath();
    QByteArray env = qgetenv("MSHV_DATA_DIR");
    if (!env.isEmpty())
    {
        QString data = QDir::fromNativeSeparators(QString::fromLocal8Bit(env));
        QByteArray seed = qgetenv("MSHV_SEED_DIR");
        QString sdir = seed.isEmpty() ? exe_dir : QDir::fromNativeSeparators(QString::fromLocal8Bit(seed));
        if (QDir(sdir).absolutePath() != QDir(data).absolutePath())
        {
            QStringList sub;
            sub << "settings" << "log" << "RxWavs" << "ExportLog" << "AllTxtMonthly" << "Screenshots";
            for (int i = 0; i < sub.count(); ++i) MshvCopyTreeNoOverwrite(sdir+"/"+sub.at(i),data+"/"+sub.at(i));
        }
        QDir().mkpath(data);
        cached = data;
        return cached;
    }
#if defined _MACOS_ && !defined _OPENBSDHV_
    QString data = QDir::homePath()+"/Library/Application Support/MSHV-IC8TEM";
    QString seed = QDir(exe_dir+"/../Resources").absolutePath();
    QStringList sub;
    sub << "settings" << "log" << "RxWavs" << "ExportLog" << "AllTxtMonthly" << "Screenshots";
    for (int i = 0; i < sub.count(); ++i)
    {
        MshvCopyTreeNoOverwrite(seed+"/"+sub.at(i),data+"/"+sub.at(i));
        QDir().mkpath(data+"/"+sub.at(i));
    }
    cached = data;
#else
    // IC8TEM: original behaviour (data next to the executable) when that folder is writable;
    // otherwise (e.g. installed in Program Files or /opt) use a per-user folder seeded from it.
    {
        QFile wt(exe_dir+"/.mshv_write_test");
        if (wt.open(QIODevice::WriteOnly))
        {
            wt.close();
            wt.remove();
            cached = exe_dir;
            return cached;
        }
    }
#if defined _WIN32_ || defined _WIN64_
    QByteArray ad = qgetenv("APPDATA");
    QString data = (ad.isEmpty() ? QDir::homePath() : QDir::fromNativeSeparators(QString::fromLocal8Bit(ad)))+"/MSHV-IC8TEM";
#else
    QString data = QDir::homePath()+"/.local/share/MSHV-IC8TEM";
#endif
    QStringList sub;
    sub << "settings" << "log" << "RxWavs" << "ExportLog" << "AllTxtMonthly" << "Screenshots";
    for (int i = 0; i < sub.count(); ++i)
    {
        MshvCopyTreeNoOverwrite(exe_dir+"/"+sub.at(i),data+"/"+sub.at(i));
        QDir().mkpath(data+"/"+sub.at(i));
    }
    cached = data;
#endif
    return cached;
}
#endif
