function loadCharts(date=(new Date()).toISOString().slice(0,10)) {
    var file = 'h21/'+date+'.json';
    document.getElementById('datum').innerHTML=date;
    document.getElementById('csv').value='date;co2;temp;tvoc;humidity;rssi\n';
    d3.json(file).then(function(data) {
	data.forEach(function(x) {
	    x['date']=new Date(x['timestamp']);
	    x['co2']=x['co2'][0];
	    x['temp']=x['temp'][0]-6.0;
	    x['tvoc']=x['tvoc'][0];
	    x['humidity']=x['humidity'][0];
	    var exceldate = x['date'].toLocaleDateString().slice(0,10)+' '+x['date'].toTimeString().slice(0,8);
	    document.getElementById('csv').value+=exceldate+';'+x['co2']+';'+x['temp']+';'+x['tvoc']+';'+x['humidity']+';'+x['rssi']+'\n';
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
	MG.data_graphic({
            title: "LoRa Empfangsqualität im E22",
            description: "Gibt an, wie gut die LoRa-Packet empfangen werden.",
            data: data,
            width: 800,
            height: 200,
            target: '#rssichart',
            x_accessor: 'date',
            y_accessor: 'rssi',
	    brush: 'x',
	    min_y_from_data: [true,true, true]
	})
    });
}

function pocchart_init() {
    loadCharts();
    var today = new Date();
    var html="";
    for (var i=0; i>-4; i--) {
	var date = today.toISOString().slice(0,10);
	html += "<a href='#' onclick='loadCharts(\""+date+"\")'>"+date+"</a> &nbsp; ";
	today.setDate(today.getDate()-1);
    }
    document.getElementById('links').innerHTML=html;
}
