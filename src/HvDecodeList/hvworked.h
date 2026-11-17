/* MSHV-IC8TEM: callsigns worked in the whole log (for the pink "worked on other bands" highlight).
 * Filled by HvLogW, read by DecodeList (both in the GUI thread).
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVWORKED_H
#define HVWORKED_H
#include <QSet>
#include <QString>
struct HvWorked
{
    static QSet<QString> &Calls() { static QSet<QString> s; return s; }   // every call in the log (after the fictitious reset)
    static bool &GridsReady() { static bool f = false; return f; }       // the worked-grid list is valid for this mode
};
#endif
