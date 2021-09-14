# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in /Users/aiven/Library/Android/sdk/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

-keep class org.simple.** { *; }



-keep interface org.simple.** { *; }

-keepclassmembers class *{

        @org.simple.eventbus.Subscriber <methods>;

        }

-keepattributes *Annotation*

-keep class   com.joyhonest.wifination.**{ *; }

-keep class   edu.emu.pocketsphinx.**{ *; }

-keep interface edu.emu.pocketsphinx.** { *; }

-keep class com.joyhonest.jh_ui.JH_App { *; }



#3D 地图 V5.0.0之后：
-keep   class com.amap.api.maps.**{*;}
-keep   class com.autonavi.**{*;}
-keep   class com.amap.api.trace.**{*;}

#定位
-keep class com.amap.api.location.**{*;}
-keep class com.amap.api.fence.**{*;}
-keep class com.autonavi.aps.amapapi.model.**{*;}

#搜索
-keep   class com.amap.api.services.**{*;}


-keep class com.baidu.** {*;}
-keep class vi.com.** {*;}
-dontwarn com.baidu.**

