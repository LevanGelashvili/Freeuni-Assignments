package com.example.weather_application

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView

class RecyclerAdapter: RecyclerView.Adapter<RecyclerView.ViewHolder>() {

    private val data: MutableList<Models.HourlyWeather> = mutableListOf()

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RecyclerView.ViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.cell_item, parent, false)
        return CellViewHolder(view)
    }

    override fun getItemCount(): Int {
        return data.size
    }

    override fun onBindViewHolder(holder: RecyclerView.ViewHolder, position: Int) {
        val cellViewHolder = holder as CellViewHolder
        cellViewHolder.setupView(data[position])
    }

    fun setData(data: List<Models.HourlyWeather>) {
        this.data.clear()
        this.data.addAll(data)
        notifyDataSetChanged()
    }
}