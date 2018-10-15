d3.json('pocdata.json').then(function(data) {
    data.forEach(function(x) {x['date']=new Date(x['timestamp'])});
    MG.data_graphic({
        title: "ppm CO2, ppb TVOC im H21",
        description: "ppm CO2, ppb TVOC im H21",
        data: data,
        width: 800,
        height: 450,
        target: '#pocchart',
        x_accessor: 'date',
        y_accessor: ['co2', 'temp', 'tvoc'],
        markers: [{'date': "2018-09-20T-07:55:00Z+2", 'label': 'L1'}],
	min_y_from_data: [true,true, true]
    })
});
