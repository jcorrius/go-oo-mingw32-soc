/*
 *   OpenOffice.org extension for syntax highlighting
 *   Copyright (C) 2008  Cédric Bosdonnat cedricbosdo@openoffice.org
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; 
 *   version 2 of the License.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
package org.openoffice.coooder.comp.ui;

import com.sun.star.awt.InvalidateStyle;
import com.sun.star.awt.XControl;
import com.sun.star.awt.XProgressBar;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameContainer;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

/**
 * Progress monitor implementation showing the progress as a dialog
 * containing a progress bar.
 * 
 * The progress is monitored using a percentage value regularly 
 * updated by the monitored process.
 * 
 * @author cbosdonnat
 *
 */
public class ProgressDialog extends AbstractDialog {

    private static final String LABEL_NAME = "lblDescription";
    private static final String BAR_NAME = "barProgress";

    public ProgressDialog(XComponentContext pContext) {
        super(pContext, "Highlighting progress");
    }
    
    /**
     * Asynchronously launch the dialog.
     */
    public void execute() {
        
        // Runs the dialog in a separate thread
        new Thread(new Runnable() {
            public void run() {
                doExecute();
            }
        }).start();
    }
    
    protected int[] getDialogBounds() {
        return new int[]{120, 120, 40, 180};
    }
    
    protected void createControls(XControl pDlgControl) throws Exception {
        XMultiServiceFactory factory = getFactory();
        XNameContainer modelContainer = getModelContainer();

        Object label = factory.createInstance("com.sun.star.awt.UnoControlFixedTextModel");
        XPropertySet labelProps = setBounds(label, 5, 5, 10, 170);
        labelProps.setPropertyValue("Label", "Syntax highlighting progress");
        labelProps.setPropertyValue("Name", LABEL_NAME);
        
        modelContainer.insertByName(LABEL_NAME, label);
        
        Object bar = factory.createInstance("com.sun.star.awt.UnoControlProgressBarModel");
        XPropertySet barProps = setBounds(bar, 5, 20, 15, 170);
        barProps.setPropertyValue("ProgressValueMin", new Integer(0));
        barProps.setPropertyValue("ProgressValueMax", new Integer(100));
        
        modelContainer.insertByName(BAR_NAME, bar);
    }

    /**
     * Asynchronously updates the dialog's progress bar using the given value.
     * 
     * @param pValue the progress value to show in percent.
     */
    public void updateProgress(final int pValue) {
        
        // Updates the dialog progress asynchronously
        
        new Thread(new Runnable(){
            public void run() {
                XControl bar = getControlContainer().getControl(BAR_NAME);
                XProgressBar xProgressBar = (XProgressBar)UnoRuntime.queryInterface(
                        XProgressBar.class, bar);
                
                try {
                    xProgressBar.setValue(pValue);
                    XControl dlg = (XControl)UnoRuntime.queryInterface(XControl.class, getControlContainer());
                    dlg.getPeer().invalidate(InvalidateStyle.UPDATE);
                    if (pValue == 100) {
                        doEndExecute();
                    }
                } catch (Exception e) {
                    // Nothing to show
                    e.printStackTrace();
                }
            }
        }).start();
    }
}
