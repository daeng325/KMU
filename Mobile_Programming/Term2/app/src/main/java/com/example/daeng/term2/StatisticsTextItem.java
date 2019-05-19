package com.example.daeng.term2;

/**
 * Created by daeng on 2016-11-28.
 */

public class StatisticsTextItem {

    /**
     * Data array
     */
    private String mData;

    /**
     * True if this item is selectable
     */
    private boolean mSelectable = true;

    /**
     * Initialize with icon and data array

     */
    public StatisticsTextItem(String obj) {
        mData = obj;
    }


    /**
     * True if this item is selectable
     */
    public boolean isSelectable() {
        return mSelectable;
    }

    /**
     * Set selectable flag
     */
    public void setSelectable(boolean selectable) {
        mSelectable = selectable;
    }

    /**
     * Get data array
     *
     * @return
     */
    public String getData() {
        return mData;
    }

    /**
     * Get data
     */
    public String getData(int index) {
        if (mData == null) {
            return null;
        }
        return mData;
    }

    /**
     * Set data array
     *
     * @param obj
     */
    public void setData(String obj) {
        mData = obj;
    }
}
