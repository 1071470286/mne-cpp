//=============================================================================================================
/**
 * @file     coregview.h
 * @author   Ruben Dörfel <doerfelruben@aol.com>
 * @since    0.1.0
 * @date     August, 2020
 *
 * @section  LICENSE
 *
 * Copyright (C) 2020, Ruben Dörfel. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 * the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *       following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 *       the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @brief     DISPLIB class declaration.
 *
 */

#ifndef COREGVIEW_H
#define COREGVIEW_H

//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../disp_global.h"
#include "abstractview.h"

//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QSharedPointer>

//=============================================================================================================
// EIGEN INCLUDES
//=============================================================================================================

//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

namespace Ui {
    class CoregViewWidget;
}

//=============================================================================================================
// DEFINE NAMESPACE NAMESPACE
//=============================================================================================================

namespace DISPLIB
{

//=============================================================================================================
// NAMESPACE FORWARD DECLARATIONS
//=============================================================================================================

//=============================================================================================================
/**
 * Description of what this class is intended to do (in detail).
 *
 * @brief Brief description of this class.
 */
class DISPSHARED_EXPORT CoregView : public AbstractView
{
    Q_OBJECT

public:
    typedef QSharedPointer<CoregView> SPtr;            /**< Shared pointer type for DISPLIB. */
    typedef QSharedPointer<const CoregView> ConstSPtr; /**< Const shared pointer type for DISPLIB. */

    //=========================================================================================================
    /**
    * Constructs a CoregView object.
    */
    explicit CoregView(const QString& sSettingsPath = "",
                             QWidget *parent = 0,
                             Qt::WindowFlags f = Qt::Widget);

    ~CoregView();


protected:

private:
    Ui::CoregViewWidget*        m_pUi;                          /**< The rtFwd dialog. */
    QString                     m_sSettingsPath;                /**< The settings path to store the GUI settings to. */

signals:

};

//=============================================================================================================
// INLINE DEFINITIONS
//=============================================================================================================


} // NAMESPACE

#endif // COREGVIEW_H

