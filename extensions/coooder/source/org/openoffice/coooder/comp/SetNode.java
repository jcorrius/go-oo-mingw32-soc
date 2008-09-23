package org.openoffice.coooder.comp;

import org.openoffice.coooder.HighlightingException;

import com.sun.star.xml.dom.XNode;

public class SetNode {

    private boolean mCaseSensitive;
    private String mId;
    private String[] mValues;

    public SetNode (XNode pSetXmlNode) {

        if (pSetXmlNode.getNodeName().equals(DefinitionsConstants.NODE_SET)) {
            // Check for case sensitivity
            try {
                String value = Utils.getAttribute(pSetXmlNode, DefinitionsConstants.ATTR_CASE_SENSITIVE);
                mCaseSensitive = Boolean.parseBoolean(value);
            } catch (HighlightingException e) {
                // Default value is true
                mCaseSensitive = true;
            }

            // Get the keyword set id
            try {
                mId = Utils.getAttribute(pSetXmlNode, DefinitionsConstants.ATTR_ID);

                // Get the keywords
                mValues = Utils.getValues(pSetXmlNode);
            } catch (HighlightingException e) {
                // No ID: no keywordset
            }
        }
    }
    
    public boolean isCaseSensitive() {
        return mCaseSensitive;
    }
    
    public String getId() {
        return mId;
    }
    
    public String[] getValues() {
        return mValues;
    }
}
