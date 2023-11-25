package com.example.demoiotdashboard.alert;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.view.View;

public class Alerts {

    // Player has not selected a team
    public void noConnection(View view) {
        AlertDialog.Builder builder = new AlertDialog.Builder(view.getContext());
        builder.setMessage("Select your team.");
        builder.setCancelable(true);

        builder.setNeutralButton(
                "Okay",
                (dialog, id) -> dialog.cancel());

        AlertDialog a = builder.create();
        a.show();
    }
}