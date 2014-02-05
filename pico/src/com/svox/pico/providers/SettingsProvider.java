/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.svox.pico.providers;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.net.Uri;
import android.os.Environment;

/*
 * This content provider enables the TtsService to get a String of configuration
 * data from the plugin engine and pass it back to that engine's .so file in the
 * native layer.
 *
 * In this particular case, the only configuration information being passed is
 * the location of the data files for the Pico engine which live under
 *     /<external storage>/svox/
 *
 */

public class SettingsProvider extends ContentProvider {
    private class SettingsCursor extends MatrixCursor {
        private String settings;

        public SettingsCursor(String[] columnNames) {
            super(columnNames);
        }

        public void putSettings(String settings){
            this.settings = settings;
        }

        @Override
        public int getCount(){
            return 1;
        }

        @Override
        public String getString(int column){
            return settings;
        }
    }


    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        return 0;
    }

    @Override
    public String getType(Uri uri) {
        return null;
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        return null;
    }

    @Override
    public boolean onCreate() {
        return true;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs,
            String sortOrder) {
        String[] dummyColumns = {"", ""};
        SettingsCursor cursor = new SettingsCursor(dummyColumns);
        cursor.putSettings(Environment.getExternalStorageDirectory() + "/svox/");
        return cursor;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
        return 0;
    }

}
