package com.example.weather_application

import android.view.View
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class CellViewHolder(view: View) : RecyclerView.ViewHolder(view) {

    private var tempText: TextView = view.findViewById(R.id.cell_temp)
    private var timeText: TextView = view.findViewById(R.id.cell_time)
    private var iconImage: ImageView = view.findViewById(R.id.cell_icon)

    fun setupView(weather: Models.HourlyWeather) {
        timeText.text = Utils.formatUnixTime(weather.dt, " E d  h:mma")
        tempText.text = Utils.toCelsius(weather.main.temp)
        Utils.loadPicassoImage(weather.weather[0].icon, iconImage)
    }
}