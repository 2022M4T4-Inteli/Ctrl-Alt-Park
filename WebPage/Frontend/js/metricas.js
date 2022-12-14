var url = "http://10.128.64.59:2707";

//Function to convert hour to minutes and calculate the absolute vallet time
function convertToMinutes(time1, time2){
    var firstMin = time1.getHours() * 60;
    var firstMin = firstMin + time1.getMinutes();

    var lastMin = time2.getHours() * 60;
    var lastMin = lastMin + time2.getMinutes();

    var absoluteMinutes = lastMin - firstMin;

    return absoluteMinutes;
}

//Function that execute every 100 milliseconds
setInterval(async function(){

    //Request to get all actives vallets to include on totem
    $.get(url + "/getAllVallets", function (resultado) {
        var objeto = JSON.parse(resultado);

        var parkingTime = 0;
        var returningTime = 0;

        //Removing datas that don't match with required status
        for (i = 0; i < Object.keys(objeto).length; i++) {
            if (objeto[i].STATUS != "Carro devolvido"){
                delete objeto[i];
            }
        }

        //Reseting array index
        objeto = objeto.filter(function(){return true;});

        //For to insert all the data on a table
        for (i = 0; i < Object.keys(objeto).length; i++) {
            Object.assign(objeto[i], {"TIME1": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP1)), new Date(Date.parse(objeto[i].TIME2_VP1)))});
            Object.assign(objeto[i], {"TIME2": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP2)), new Date(Date.parse(objeto[i].TIME2_VP2)))});

            parkingTime += objeto[i].TIME1;
            returningTime += objeto[i].TIME2;
        }

        parkingTime = parkingTime / Object.keys(objeto).length;
        returningTime = returningTime / Object.keys(objeto).length;

        $(".parkingTime").html(parkingTime.toFixed(0) + " min");
        $(".returningTime").html(returningTime.toFixed(0) + " min");
    });
}, 1000);