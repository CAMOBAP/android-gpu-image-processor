package com.camobap.imgmixer

import android.content.Context
import android.content.res.AssetManager
import android.util.Log

import java.io.File
import java.io.FileOutputStream
import java.io.IOException

/**
 * Motivation:
 * Android NDK have horrible native API for resources in assets
 * https://developer.android.com/ndk/reference/group/asset
 *
 * Solution:
 * To make existing code work with c/cpp fs API we will copy assets content to
 * some directory which accessible from native side for instance `<sandbox>/files`
 *
 * How to check content files
 * adb shell run-as com.camobap.imgmixer ls -Ral files/
</sandbox> */
internal object AssetsExtractor {
    private val TAG = AssetsExtractor::class.java.simpleName

    @Throws(IOException::class)
    private fun extractFile(
        assetManager: AssetManager, assetPath: String,
        outFile: File, override: Boolean
    ) {
        if (outFile.exists()) {
            if (override) {
                if (!outFile.delete()) {
                    Log.w(TAG, "Cannot delete $outFile during assets extraction")
                }
            } else {
                return
            }
        }

        assetManager.open(assetPath).use { inn ->
            FileOutputStream(outFile).use { out ->
                inn.copyTo(out, 1024)
                out.flush()
            }
        }
    }

    @Throws(IOException::class)
    fun extractDir(context: Context, assetPath: String, target: File, override: Boolean): Boolean {
        val assetManager = context.assets

        return traverseAssets(assetManager, assetPath, File(target, assetPath), override)
    }

    @Throws(IOException::class)
    private fun traverseAssets(
        manager: AssetManager, sourcePath: String,
        destination: File, override: Boolean
    ): Boolean {
        try {
            val list = manager.list(sourcePath)

            if (list != null && list.size > 0) {
                if (destination.exists() || destination.mkdirs()) {
                    for (file in list) {
                        val path = if (sourcePath.length == 0) file else "$sourcePath/$file"

                        if (!traverseAssets(manager, path, File(destination, file), override)) {
                            return false
                        }
                    }
                } else {
                    Log.w(TAG, "Cannot create $destination during assets extraction")
                }
            } else {
                extractFile(manager, sourcePath, destination, override)
            }
        } catch (e: IOException) {
            Log.e(TAG, "Cannot extract $sourcePath from assets", e)

            throw e
        }

        return true
    }
}
