package com.example.weather_application

import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.fragment.app.FragmentPagerAdapter
import com.example.weather_application.fragments.ParentFragment

class ViewPagerAdapter(fm: FragmentManager, behavior: Int, countries: List<Models.Country>):
    FragmentPagerAdapter(fm, behavior) {

    private val countries: List<Models.Country> = countries

    override fun getItem(position: Int): Fragment {
        return ParentFragment.newInstance(countries[position].name)
    }

    override fun getCount(): Int {
        return countries.size
    }
}