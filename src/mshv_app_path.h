/* MSHV-IC8TEM: compatibility with the macOS port of VU2CPL.
 * His macOS code asks mshv_app_data_path() for the writable data folder;
 * in MSHV-IC8TEM that is MshvAppPath() (see mshv_ic8tem_path.h):
 * ~/Library/Application Support/MSHV-IC8TEM on macOS.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef MSHV_APP_PATH_H
#define MSHV_APP_PATH_H
#include "mshv_ic8tem_path.h"
static inline QString mshv_app_data_path()
{
    return MshvAppPath();
}
#endif
