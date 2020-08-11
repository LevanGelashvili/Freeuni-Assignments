package com.nav.myapplication;

import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class MainActivity extends AppCompatActivity {

    private static final int DELAY_TIME = 500;
    private static final Integer[] R_PICTURES = {R.drawable.andy_morin, R.drawable.beyonce, R.drawable.elza, R.drawable.mc_ride, R.drawable.robert_pattinson, R.drawable.zach_hill};
    private static int flipsCount = 0;
    private static boolean anyCardsOpen = false;
    private static boolean canClick = true;
    private ConstraintLayout mainContainer;
    private TextView textView;
    private Map<ImageView, Integer> viewPictures;
    private View lastChosenView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.content_main);
        mainContainer = findViewById(R.id.ImageConstraintLayout);
        textView = findViewById(R.id.flip_text);
        viewPictures = initPictures(mainContainer);

        for (int i = 0; i < mainContainer.getChildCount(); i++) {
            mainContainer.getChildAt(i).setOnClickListener(onClickListener);
        }
    }


    private View.OnClickListener onClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            if (!canClick || !(v instanceof ImageView) || v.getVisibility() == View.INVISIBLE || v == lastChosenView) return;

            textView.setText("Flips : " + (++flipsCount));
            ((ImageView) v).setImageResource(viewPictures.get(v));

            if (anyCardsOpen) {
                handleOpenCard(v);
            } else {
                lastChosenView = v;
            }
            anyCardsOpen = !anyCardsOpen;
            }
    };


    private void handleOpenCard(final View currentView) {

        canClick = false;

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
            if (viewPictures.get(lastChosenView).equals(viewPictures.get(currentView))) {
                lastChosenView.setVisibility(View.INVISIBLE);
                currentView.setVisibility(View.INVISIBLE);
            } else {
                ((ImageView) lastChosenView).setImageResource(R.drawable.deck);
                ((ImageView) currentView).setImageResource(R.drawable.deck);
            }

            canClick = true;
            lastChosenView = null;

            }
        }, DELAY_TIME);
    }


    private Map<ImageView, Integer> initPictures(ConstraintLayout mainContainer) {

        List<Integer> pictures = new ArrayList<>();
        for (int pic : R_PICTURES) {
            pictures.add(pic);
            pictures.add(pic);
        }
        Collections.shuffle(pictures);

        Map<ImageView, Integer> table = new HashMap<>();
        for (int i = 0; i < mainContainer.getChildCount(); i++) {
            table.put((ImageView) mainContainer.getChildAt(i), pictures.get(i));
        }

        return table;
    }
}
