package com.example.weather_application

import android.graphics.Rect
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.RecyclerView.ItemDecoration
import com.example.weather_application.Models.CurrentWeather
import com.example.weather_application.Models.Forecast
import com.example.weather_application.Utils.WaitingModes
import com.example.weather_application.Utils.formatUnixTime
import com.example.weather_application.Utils.toCelsius
import com.example.weather_application.networking.CurrentWeatherClient
import com.example.weather_application.networking.ForecastClient
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response
import java.text.SimpleDateFormat
import java.util.*

private const val COUNTRY = "country"

class ParentFragment : Fragment() {

    private var country: String? = null
    private var weather: CurrentWeather? = null
    private var forecast: Forecast? = null
    private lateinit var savedView: View
    private lateinit var recyclerView: RecyclerView
    private lateinit var adapter: RecyclerAdapter

    companion object {
        fun newInstance(country: String): ParentFragment {
            val args = Bundle()
            val fragment = ParentFragment()
            args.putString(COUNTRY, country)
            fragment.arguments = args
            return fragment
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (savedInstanceState == null)
            country = arguments!!.getString(COUNTRY)
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        savedView = inflater.inflate(R.layout.parent_fragment, container, false)
        setRecyclerParameters()
        changeWaiting(WaitingModes.WAITING)
        getWeather(country!!)
        return savedView
    }

    private fun updateView() {
        val date = Date(weather!!.dt.toLong() * 1000)
        val isDay = date.hours in 6..17
        initGradient(savedView, weather!!, isDay, date)
        initParams(savedView, weather!!, isDay)
    }

    private fun initGradient(view: View, curWeather: CurrentWeather, isDay: Boolean, date: Date) {
        val layout = view.findViewById<ConstraintLayout>(R.id.parent_fragment_id)
            .findViewById<ConstraintLayout>(R.id.grad_id_fragment)
        layout.setBackgroundResource(if (isDay) R.drawable.gradient_day else R.drawable.gradient_night)

        view.findViewById<ImageView>(R.id.fragment_gradient_icon)
            .setImageResource(if (isDay) R.drawable.ic_sun else R.drawable.ic_moon)

        view.findViewById<TextView>(R.id.fragment_gradient_country).text = country
        view.findViewById<TextView>(R.id.fragment_gradient_date).text =
            SimpleDateFormat("E d").format(date)
        view.findViewById<TextView>(R.id.fragment_gradient_time).text =
            SimpleDateFormat("hh:mm a").format(date).toLowerCase()

        view.findViewById<TextView>(R.id.fragment_gradient_temperature).text =
            toCelsius(curWeather.main.temp)
        view.findViewById<TextView>(R.id.fragment_gradient_perceived).text =
            "Perceived ${toCelsius(curWeather.main.feels_like)}"
    }

    private fun initParams(view: View, curWeather: CurrentWeather, isDay: Boolean) {
        view.findViewById<TextView>(R.id.param_precipitation).text = "${curWeather.clouds.all}%"
        view.findViewById<TextView>(R.id.param_humidity).text = "${curWeather.main.humidity}%"
        view.findViewById<TextView>(R.id.param_windspeed).text = "${curWeather.wind.speed}kmh"
        val sunrise = formatUnixTime(curWeather.sys.sunrise, "h:mma")
        val sunset = formatUnixTime(curWeather.sys.sunset, "h:mma")
        view.findViewById<TextView>(R.id.param_day_night).text = "$sunrise$sunset".toLowerCase()

        if (isDay) {
            view.findViewById<ImageView>(R.id.param_drop).setImageResource(R.drawable.ic_drop)
            view.findViewById<ImageView>(R.id.param_three_drops)
                .setImageResource(R.drawable.ic_humidity)
            view.findViewById<ImageView>(R.id.param_flag).setImageResource(R.drawable.ic_flag)
        } else {
            view.findViewById<ImageView>(R.id.param_drop).setImageResource(R.drawable.night_drop)
            view.findViewById<ImageView>(R.id.param_three_drops)
                .setImageResource(R.drawable.night_three_drop)
            view.findViewById<ImageView>(R.id.param_flag).setImageResource(R.drawable.night_flag)
        }
        view.findViewById<ImageView>(R.id.param_sun_moon).setImageResource(R.drawable.ic_day_night)
    }

    private fun initRecycler() {
        adapter.setData(forecast!!.list.subList(1, forecast!!.list.size))
    }

    private fun getForecast(country: String?) {
        val call: Call<Forecast> = ForecastClient.getClient.getForecast(country, Utils.API_KEY)
        call.enqueue(object : Callback<Forecast> {

            override fun onResponse(call: Call<Forecast>?, response: Response<Forecast>?) {
                if (response!!.isSuccessful) {
                    forecast = response.body()
                    initRecycler()
                }
            }

            override fun onFailure(call: Call<Forecast>?, t: Throwable?) {
                t!!.printStackTrace()
            }
        })
    }

    private fun getWeather(country: String?) {
        val call: Call<CurrentWeather> =
            CurrentWeatherClient.getClient.getCurrentWeather(country, Utils.API_KEY)
        call.enqueue(object : Callback<CurrentWeather> {

            override fun onResponse(
                call: Call<CurrentWeather>,
                response: Response<CurrentWeather>
            ) {
                if (response.isSuccessful) {
                    changeWaiting(WaitingModes.RECEIVED_RESPONSE)
                    weather = response.body()!!
                    updateView()
                    getForecast(country)
                } else {
                    changeWaiting(WaitingModes.NO_RESPONSE)
                }
            }

            override fun onFailure(call: Call<CurrentWeather>, t: Throwable) {
                t.printStackTrace()
            }
        })
    }

    private fun changeWaiting(mode: WaitingModes) {
        when (mode) {
            WaitingModes.WAITING -> {
                savedView.findViewById<TextView>(R.id.waiting_txt).visibility = View.VISIBLE
                savedView.findViewById<TextView>(R.id.waiting_txt).text = "Waiting $country..."
            }
            WaitingModes.NO_RESPONSE -> {
                savedView.findViewById<TextView>(R.id.waiting_txt).visibility = View.VISIBLE
                savedView.findViewById<TextView>(R.id.waiting_txt).text =
                    "No response from $country"
            }
            else -> {
                savedView.findViewById<TextView>(R.id.waiting_txt).visibility = View.INVISIBLE
            }
        }
    }

    private fun setRecyclerParameters() {
        adapter = RecyclerAdapter()
        recyclerView = savedView.findViewById(R.id.recycler_view)
        recyclerView.adapter = adapter
        recyclerView.layoutManager = LinearLayoutManager(savedView.context)
        val dividerItemDecoration = DividerItemDecoration(context, DividerItemDecoration.VERTICAL);
        dividerItemDecoration.setDrawable(ContextCompat.getDrawable(context!!, R.drawable.divider)!!)
        recyclerView.addItemDecoration(dividerItemDecoration)
    }

}