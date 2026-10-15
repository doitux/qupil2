// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.app;

import android.Manifest;
import android.app.Activity;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.time.Instant;
import java.time.LocalDate;
import java.time.ZoneId;

/**
 * Lesson-end alerts while Qupil is in the background or not running at all.
 *
 * Qupil hands over the weekly alert times with schedule(). They are kept in
 * the shared preferences and exactly one alarm is set: the next alert. When
 * it goes off, onReceive() sets the alarm for the following alert, shows the
 * notification and plays the sound chosen in Qupil (with its volume already
 * applied). After a restart of the device, an update of Qupil or a change of
 * the clock or time zone the alarm is set again.
 */
public class LessonAlarms extends BroadcastReceiver {
    private static final String TAG = "QupilLessonAlarms";
    private static final String PREFS = "qupil_lesson_alarms";
    private static final String CHANNEL = "lesson_end";
    private static final String ACTION_ALERT = "de.qupil.app.action.LESSON_END";
    private static final int NOTIFICATION_ID = 4710;
    private static final int REQUEST_NOTIFICATIONS = 47110;
    private static final int MINUTES_PER_DAY = 24 * 60;
    private static final int ACCENT = 0xFF6E3C5A;

    // ---------------------------------------------------------------- called by Qupil (C++)

    /**
     * plan: {"sound": "/…/lesson-end-….wav", "channel": "End of lesson",
     *        "alerts": [{"at": minute of the week (Monday 00:00 = 0), "lead": minutes, "title": "…", "text": "…"}]}
     */
    public static void schedule(Context context, String plan) {
        Context app = context.getApplicationContext();
        prefs(app).edit().putString("plan", plan).apply();
        try {
            createChannel(app, new JSONObject(plan).optString("channel", "Qupil"));
        } catch (JSONException e) {
            Log.w(TAG, "Unreadable plan", e);
        }
        setNextAlarm(app);
    }

    public static void cancel(Context context) {
        Context app = context.getApplicationContext();
        prefs(app).edit().remove("plan").apply();
        alarmManager(app).cancel(alarmIntent(app, null, 0));
    }

    /** Bit 0: notifications are shown; bit 1: exact alarms allowed; bit 2: Qupil may (still) ask */
    public static int state(Context context) {
        int state = 0;
        NotificationManager manager = context.getSystemService(NotificationManager.class);
        if (manager != null && manager.areNotificationsEnabled()) {
            NotificationChannel channel = manager.getNotificationChannel(CHANNEL);
            if (channel == null || channel.getImportance() != NotificationManager.IMPORTANCE_NONE)
                state |= 1;
        } else if (Build.VERSION.SDK_INT >= 33 && mayAsk(context)) {
            state |= 4;
        }
        if (canSetExact(context.getApplicationContext()))
            state |= 2;
        return state;
    }

    /** Android 13+: the system question; otherwise (or when it may not ask again) the settings */
    public static void requestPermission(Context context) {
        if (Build.VERSION.SDK_INT >= 33 && context instanceof Activity && mayAsk(context)) {
            final Activity activity = (Activity) context;
            prefs(activity).edit().putBoolean("asked", true).apply();
            activity.runOnUiThread(() -> activity.requestPermissions(
                    new String[] { Manifest.permission.POST_NOTIFICATIONS }, REQUEST_NOTIFICATIONS));
        } else {
            openSettings(context, false);
        }
    }

    /** The app's notification settings, or "Alarms & reminders" (Android 12+) */
    public static void openSettings(Context context, boolean exactAlarms) {
        String pkg = context.getPackageName();
        Intent intent;
        if (exactAlarms && Build.VERSION.SDK_INT >= 31) {
            intent = new Intent(Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM, Uri.parse("package:" + pkg));
        } else {
            intent = new Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS);
            intent.putExtra(Settings.EXTRA_APP_PACKAGE, pkg);
        }
        if (!start(context, intent))
            start(context, new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS, Uri.parse("package:" + pkg)));
    }

    // ---------------------------------------------------------------- alarm

    @Override
    public void onReceive(Context context, Intent intent) {
        // Whatever happens below: the next alert is set first
        setNextAlarm(context);
        if (ACTION_ALERT.equals(intent.getAction()))
            alert(context, intent, goAsync());
        // Otherwise: device started, Qupil updated, clock changed, exact alarms allowed
    }

    private static void setNextAlarm(Context context) {
        Context app = context.getApplicationContext();
        AlarmManager alarms = alarmManager(app);
        String plan = prefs(app).getString("plan", null);
        if (plan == null) {
            alarms.cancel(alarmIntent(app, null, 0));
            return;
        }
        try {
            JSONArray alerts = new JSONObject(plan).getJSONArray("alerts");
            // A second later: an alarm that just went off is not set again
            long after = System.currentTimeMillis() + 1000;
            JSONObject next = null;
            long nextAt = Long.MAX_VALUE;
            for (int i = 0; i < alerts.length(); ++i) {
                JSONObject alert = alerts.getJSONObject(i);
                long at = nextTime(alert.getInt("at"), after);
                if (at < nextAt) {
                    nextAt = at;
                    next = alert;
                }
            }
            if (next == null) {
                alarms.cancel(alarmIntent(app, null, 0));
                return;
            }
            PendingIntent pending = alarmIntent(app, next, nextAt + next.optInt("lead", 0) * 60000L);
            if (canSetExact(app))
                alarms.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, nextAt, pending);
            else
                alarms.setAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, nextAt, pending);   // may come late
        } catch (JSONException | SecurityException e) {
            Log.w(TAG, "Could not set the alarm", e);
        }
    }

    /** The first time after `after` (ms) of the weekly alert at minute `at` of the week (Monday 00:00 = 0) */
    static long nextTime(int at, long after) {
        ZoneId zone = ZoneId.systemDefault();
        LocalDate today = Instant.ofEpochMilli(after).atZone(zone).toLocalDate();
        int weekday = at / MINUTES_PER_DAY;   // 0 = Monday
        int minute = at % MINUTES_PER_DAY;
        for (int d = 0; d <= 7; ++d) {
            LocalDate day = today.plusDays(d);
            if (day.getDayOfWeek().getValue() - 1 != weekday)
                continue;
            long time = day.atTime(minute / 60, minute % 60).atZone(zone).toInstant().toEpochMilli();
            if (time > after)
                return time;
        }
        return Long.MAX_VALUE;
    }

    private static PendingIntent alarmIntent(Context context, JSONObject alert, long end) {
        Intent intent = new Intent(context, LessonAlarms.class);
        intent.setAction(ACTION_ALERT);
        if (alert != null) {
            intent.putExtra("title", alert.optString("title"));
            intent.putExtra("text", alert.optString("text"));
            intent.putExtra("end", end);
        }
        return PendingIntent.getBroadcast(context, 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE);
    }

    // ---------------------------------------------------------------- notification and sound

    private static void alert(Context context, Intent intent, PendingResult result) {
        long end = intent.getLongExtra("end", 0);
        long now = System.currentTimeMillis();
        NotificationManager manager = context.getSystemService(NotificationManager.class);
        // Too late (the device was off) or notifications are turned off
        if ((end > 0 && now >= end) || manager == null || (state(context) & 1) == 0) {
            result.finish();
            return;
        }

        Notification.Builder builder = new Notification.Builder(context, CHANNEL)
                .setSmallIcon(icon(context))
                .setContentTitle(intent.getStringExtra("title"))
                .setContentText(intent.getStringExtra("text"))
                .setCategory(Notification.CATEGORY_REMINDER)
                .setColor(ACCENT)
                .setAutoCancel(true)
                .setShowWhen(true);
        Intent open = context.getPackageManager().getLaunchIntentForPackage(context.getPackageName());
        if (open != null) {
            builder.setContentIntent(PendingIntent.getActivity(context, 0, open,
                    PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE));
        }
        if (end > now)
            builder.setTimeoutAfter(end - now);   // gone when the lesson is over
        try {
            manager.notify(NOTIFICATION_ID, builder.build());
        } catch (SecurityException e) {
            Log.w(TAG, "Notification not allowed", e);
        }
        playSound(context, result);
    }

    private static void playSound(Context context, final PendingResult result) {
        String sound = null;
        try {
            sound = new JSONObject(prefs(context).getString("plan", "{}")).optString("sound", null);
        } catch (JSONException e) {
            Log.w(TAG, "Unreadable plan", e);
        }
        final AudioManager audio = context.getSystemService(AudioManager.class);
        NotificationManager manager = context.getSystemService(NotificationManager.class);
        // Silent or vibrate, "Do not disturb": no sound (the notification may still vibrate)
        boolean quiet = audio == null || audio.getRingerMode() != AudioManager.RINGER_MODE_NORMAL
                || (manager != null && manager.getCurrentInterruptionFilter() > NotificationManager.INTERRUPTION_FILTER_ALL);
        if (quiet || sound == null || sound.isEmpty() || !new File(sound).isFile()) {
            result.finish();
            return;
        }

        final AudioAttributes attributes = new AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_NOTIFICATION)
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                .build();
        // Music playing on the device gets quieter for the moment
        final AudioFocusRequest focus = new AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK)
                .setAudioAttributes(attributes)
                .build();
        final MediaPlayer player = new MediaPlayer();
        final boolean[] finished = { false };
        final Runnable finish = () -> {
            if (finished[0])
                return;
            finished[0] = true;
            player.release();
            audio.abandonAudioFocusRequest(focus);
            result.finish();
        };
        try {
            player.setAudioAttributes(attributes);
            player.setDataSource(sound);
            player.setOnCompletionListener(p -> finish.run());
            player.setOnErrorListener((p, what, extra) -> {
                finish.run();
                return true;
            });
            player.prepare();
            audio.requestAudioFocus(focus);
            player.start();
            // A receiver may run for 10 seconds; Qupil's sounds are at most that long
            new Handler(Looper.getMainLooper()).postDelayed(finish, 9500);
        } catch (Exception e) {
            Log.w(TAG, "Could not play the sound", e);
            finish.run();
        }
    }

    private static void createChannel(Context context, String name) {
        NotificationManager manager = context.getSystemService(NotificationManager.class);
        if (manager == null)
            return;
        NotificationChannel channel = new NotificationChannel(CHANNEL, name, NotificationManager.IMPORTANCE_HIGH);
        channel.setSound(null, null);   // Qupil plays the sound chosen in its settings
        channel.enableVibration(true);
        channel.setVibrationPattern(new long[] { 0, 250, 150, 250 });
        manager.createNotificationChannel(channel);   // an existing channel only takes the new name
    }

    // ---------------------------------------------------------------- helpers

    private static int icon(Context context) {
        int id = context.getResources().getIdentifier("ic_stat_qupil", "drawable", context.getPackageName());
        return id != 0 ? id : android.R.drawable.ic_popup_reminder;
    }

    private static SharedPreferences prefs(Context context) {
        return context.getSharedPreferences(PREFS, Context.MODE_PRIVATE);
    }

    private static AlarmManager alarmManager(Context context) {
        return (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
    }

    private static boolean canSetExact(Context context) {
        return Build.VERSION.SDK_INT < 31 || alarmManager(context).canScheduleExactAlarms();
    }

    /** Not asked yet, or the user said no once and Android lets Qupil ask again */
    private static boolean mayAsk(Context context) {
        if (!prefs(context).getBoolean("asked", false))
            return true;
        return context instanceof Activity
                && ((Activity) context).shouldShowRequestPermissionRationale(Manifest.permission.POST_NOTIFICATIONS);
    }

    private static boolean start(Context context, Intent intent) {
        if (!(context instanceof Activity))
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        try {
            context.startActivity(intent);
            return true;
        } catch (ActivityNotFoundException e) {
            return false;
        }
    }
}
