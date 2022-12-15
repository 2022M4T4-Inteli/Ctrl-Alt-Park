var url = "http://10.128.64.59:2707";

var arraySize = 0;

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
        var findingTime = 0;

        arraySize = Object.keys(objeto).length;

        //Removing datas that don't match with required status
        for (var i = 0; i < Object.keys(objeto).length; i++) {
            if (objeto[i].STATUS != "Carro devolvido"){
                delete objeto[i];
            }
        }

        //Reseting array index
        objeto = objeto.filter(function(){return true;});

        switch ($(".selecao1").val()){
            case "Shopping Eldorado":
                arraySize = Object.keys(objeto).length;

                //Removing datas that don't match with required operation
                for (var i = 0; i < arraySize; i++) {

                    if ($(".selecao1").val() != objeto[i].OPERATION){
                        delete objeto[i];
                    }
                }
        
                //Reseting array index
                objeto = objeto.filter(function(){return true;});

                break;
            case "Inteli":
                arraySize = Object.keys(objeto).length;

                //Removing datas that don't match with required operation
                for (var i = 0; i < arraySize; i++) {
                    if (objeto[i].OPERATION != $(".selecao1").val()){
                        delete objeto[i];
                    }
                }
        
                //Reseting array index
                objeto = objeto.filter(function(){return true;});

                break;
        }

        switch ($(".selecao2").val()){
            case "Semanal":
                arraySize = Object.keys(objeto).length;
                var actualDate = new Date();
                actualDate.setDate(actualDate.getDate() - 7);

                //Removing datas that don't match with required time
                for (var i = 0; i < arraySize; i++) {
                    var valletDate = new Date(Date.parse(objeto[i].TIME1_VP1));
                    console.log(valletDate);
                    console.log(actualDate);

                    if (actualDate > valletDate){
                        console.log(objeto[i]);
                        delete objeto[i];
                    }
                }
        
                //Reseting array index
                objeto = objeto.filter(function(){return true;});

                break;
            case "Mensal":
                arraySize = Object.keys(objeto).length;
                var actualDate = new Date();
                actualDate.setDate(actualDate.getDate() - 30);

                //Removing datas that don't match with required time
                for (var i = 0; i < arraySize; i++) {
                    var valletDate = new Date(Date.parse(objeto[i].TIME1_VP1));
                    console.log(valletDate);
                    console.log(actualDate);

                    if (actualDate > valletDate){
                        console.log(objeto[i]);
                        delete objeto[i];
                    }
                }
        
                //Reseting array index
                objeto = objeto.filter(function(){return true;});

                break;
            case "Anual":
                arraySize = Object.keys(objeto).length;
                var actualDate = new Date();
                actualDate.setDate(actualDate.getDate() - 365);

                //Removing datas that don't match with required time
                for (var i = 0; i < arraySize; i++) {
                    var valletDate = new Date(Date.parse(objeto[i].TIME1_VP1));

                    if (actualDate > valletDate){
                        delete objeto[i];
                    }
                }
        
                //Reseting array index
                objeto = objeto.filter(function(){return true;});

                break;
        }

        //For to calculate times
        for (i = 0; i < Object.keys(objeto).length; i++) {
            Object.assign(objeto[i], {"TIME1": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP1)), new Date(Date.parse(objeto[i].TIME2_VP1)))});
            Object.assign(objeto[i], {"TIME2": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP2)), new Date(Date.parse(objeto[i].TIME2_VP2)))});

            parkingTime += objeto[i].TIME1;
            returningTime += objeto[i].TIME2;
            findingTime += objeto[i].FIND_TIME;
        }

        //Getting media
        parkingTime = parkingTime / Object.keys(objeto).length;
        returningTime = returningTime / Object.keys(objeto).length;
        findingTime = findingTime / Object.keys(objeto).length;

        //Adding times to html
        $(".parkingTime").html(parkingTime.toFixed(0) + " min");
        $(".returningTime").html(returningTime.toFixed(0) + " min");
        $(".findingTime").html(findingTime.toFixed(0) + " min");
    });
}, 1000);