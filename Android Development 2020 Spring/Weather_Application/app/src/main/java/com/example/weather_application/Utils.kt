package com.example.weather_application

import android.widget.ImageView
import com.squareup.picasso.Picasso
import java.text.SimpleDateFormat
import java.util.*

object Utils {

    const val API_KEY = "99aa317ef687e1087631e5f4ea53382f"

    enum class WaitingModes {
        WAITING, RECEIVED_RESPONSE, NO_RESPONSE
    }

    fun toCelsius(kelvin: Double): String = (kelvin - 273.15).toInt().toString() + " °C"

    fun formatUnixTime(unixTime: Int, pattern: String): String = SimpleDateFormat(pattern).format(Date(unixTime.toLong() * 1000))

    fun loadPicassoImage(icon: String, image: ImageView) = Picasso.get().load("https://openweathermap.org/img/wn/${icon}@2x.png").into(image)
}