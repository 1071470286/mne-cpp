//=============================================================================================================
/**
* @file     main.cpp
* @author   Jana Kiesel <jana.kiesel@tu-ilmenau.de>
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     August, 2016
* @section  LICENSE
*
* Copyright (C) 2016, Jana Kiesel and Matti Hamalainen. All rights reserved.
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
* @brief    Example to project points on a surface and warp the surface
*
*/
//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <mne/mne_project_to_surface.h>
#include <mne/mne.h>
#include <disp3D/view3D.h>
#include <disp3D/control/control3dwidget.h>
#include <fiff/fiff_dig_point_set.h>
#include <utils/warp.h>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QApplication>
#include <QMainWindow>
#include <QCommandLineParser>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISP3DLIB;
using namespace MNELIB;
using namespace FIFFLIB;


//*************************************************************************************************************
//=============================================================================================================
// MAIN
//=============================================================================================================

//=============================================================================================================
/**
* The function main marks the entry point of the program.
* By default, main has the storage class extern.
*
* @param [in] argc (argument count) is an integer that indicates how many arguments were entered on the command line when the program was started.
* @param [in] argv (argument vector) is an array of pointers to arrays of character objects. The array objects are null-terminated strings, representing the arguments that were entered on the command line when the program was started.
* @return the value that was set to exit() (which is 0 if exit() is called via quit()).
*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Command Line Parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Projection Example");
    parser.addHelpOption();
    QCommandLineOption sampleBEMFileOption("f", "Path to BEM <file>.", "file",
                                           "D:/Studium/Master/Masterarbeit/Daten/AVG/AVG3-0Years_segmented_BEM3-1824-1860-2530-bem.fif");

    parser.addOption(sampleBEMFileOption);
    parser.process(app);

    //########################################################################################
    // Read the average BEM
    QFile t_fileBem(parser.value(sampleBEMFileOption));
    MNEBem t_Bem(t_fileBem);

    // Read Patients Bem to compare
    QFile t_fileBemPatient("D:/Studium/Master/Masterarbeit/Daten/4926787/ID_4926787_act/bem/FREESURFER-5120-5120-5120-bem.fif");
    MNEBem t_BemPatient(t_fileBemPatient);

    // Read the Digitizer
    QFile t_fileDig("D:/Studium/Master/Masterarbeit/Daten/4926787/160130_151742_4926787_auditorytoneburst_raw.fif");
    FiffDigPointSet t_Dig(t_fileDig);

    MatrixXf Dig(t_Dig.size(), 3);
    for (int i = 0; i < t_Dig.size(); ++i)
    {
        Dig(i,0) = t_Dig[i].r[0];
        Dig(i,1) = t_Dig[i].r[1];
        Dig(i,2) = t_Dig[i].r[2];
    }

    // Read and apply Transformation on Average Bem (average to head)
    QFile t_fileTrans("D:/Studium/Master/Masterarbeit/Daten/4926787/AnalysisJana/NSSP_SGM_160130_151742_4926787_auditorytoneburst_raw-ave-trans.fif");
    FiffCoordTrans t_Trans (t_fileTrans);
    t_Bem.invtransform(t_Trans);


    // Read and apply Transformation on Digitizer and Average Bem (head to MRI)
    QFile t_fileTransP("D:/Studium/Master/Masterarbeit/Daten/4926787/AnalysisJana/patient-trans.fif");
    FiffCoordTrans t_transPatient (t_fileTransP);
    t_Bem.transform(t_transPatient);

    MatrixXf DigPatient(t_Dig.size(), 3);
    DigPatient=t_transPatient.apply_trans(Dig);
    FiffDigPointSet t_DigPatient(t_Dig);

    for (int i = 0; i < t_DigPatient.size(); ++i)
    {
        t_DigPatient[i].r[0] = DigPatient(i,0);
        t_DigPatient[i].r[1] = DigPatient(i,1);
        t_DigPatient[i].r[2] = DigPatient(i,2);
    }
    //Projection
    MatrixXf DigProject(t_DigPatient.size(), 3);
    VectorXi nearest;
    VectorXf dist;
    MNEProjectToSurface t_Avg4 (t_Bem[0]);

    t_Avg4.mne_find_closest_on_surface(DigPatient, t_DigPatient.size(), DigProject, nearest, dist);

    FiffDigPointSet t_DigProject(t_Dig);

    for (int i = 0; i < t_DigProject.size(); ++i)
    {
        t_DigProject[i].r[0] = DigProject(i,0);
        t_DigProject[i].r[1] = DigProject(i,1);
        t_DigProject[i].r[2] = DigProject(i,2);
    }

    // Warp
    MNEBem t_BemWarped(t_Bem);
    t_BemWarped.warp(DigProject, DigPatient);

    //Write warped and translated Bem
    QFile t_fileBemWarped("D:/Studium/Master/Masterarbeit/Daten/4926787/AnalysisJana/warped_AVG3-0Years_segmented_BEM3-1824-1860-2530-bem.fif");
    t_BemWarped.write(t_fileBemWarped);
    t_fileBemWarped.close();

    //Write translated Bem
    QFile t_fileBemTrans("D:/Studium/Master/Masterarbeit/Daten/4926787/AnalysisJana/trans_AVG3-0Years_segmented_BEM3-1824-1860-2530-bem.fif");
    t_Bem.write(t_fileBemTrans);
    t_fileBemTrans.close();

    //Show
    View3D::SPtr testWindow = View3D::SPtr(new View3D());
    testWindow->addBemData("AVG3-0Years", "BEM", t_Bem);
    testWindow->addBemData("warped AVG3-0Years", "BEM", t_BemWarped);
    testWindow->addBemData("4926787", "BEM", t_BemPatient);
    testWindow->addDigitizerData("Original", "Dig", t_Dig);
    testWindow->addDigitizerData("warped AVG3-0Years", "Dig", t_DigPatient);
    testWindow->addDigitizerData("AVG3-0Years", "Projected Dig", t_DigProject);

    testWindow->show();

    Control3DWidget::SPtr control3DWidget = Control3DWidget::SPtr(new Control3DWidget());
    control3DWidget->setView3D(testWindow);
    control3DWidget->show();

    return app.exec();
}
