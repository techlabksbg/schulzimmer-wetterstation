d3.json('h21/'+((new Date()).toISOString().slice(0,10))+'.json').then(function(data) {
    data.forEach(function(x) {
	x['date']=new Date(x['timestamp']);
	x['co2']=x['co2'][0];
	x['temp']=x['temp'][0]-8.0;
	x['tvoc']=x['tvoc'][0];
	x['humidity']=x['humidity'][0];
    });
    MG.data_graphic({
        title: "ppm CO2 im H21",
        description: "ppm CO2 im H21",
        data: data,
        width: 800,
        height: 250,
        target: '#co2chart',
        x_accessor: 'date',
        y_accessor: 'co2',
	brush: 'x',
	min_y_from_data: [true,true, true]
    });
    MG.data_graphic({
        title: "ppb TVOC im H21",
        description: "ppb TVOC im H21",
        data: data,
        width: 800,
        height: 200,
        target: '#tvocchart',
        x_accessor: 'date',
        y_accessor: 'tvoc',
	brush: 'x',
	min_y_from_data: [true,true, true]
    })
    MG.data_graphic({
        title: "Temperatur im H21",
        description: "Temperatur im H21",
        data: data,
        width: 800,
        height: 200,
        target: '#tempchart',
        x_accessor: 'date',
        y_accessor: 'temp',
	brush: 'x',
	min_y_from_data: [true,true, true]
    })
    MG.data_graphic({
        title: "Relative Luftfeuchtigkeit im H21",
        description: "Relative Luftfeuchtigkeit im H21",
        data: data,
        width: 800,
        height: 200,
        target: '#humiditychart',
        x_accessor: 'date',
        y_accessor: 'humidity',
	brush: 'x',
	min_y_from_data: [true,true, true]
    })
});
