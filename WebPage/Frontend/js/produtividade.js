var url = "http://10.128.64.59:2707";

//This function runs when the page load
$(document).ready(function(){ 

    //Request to get all vallets
    $.get(url + "/getStatistics", function (resultado) {
        var objeto = JSON.parse(resultado);

        //Starting table title
        $("#search-table").html(`<tr id="partner-list">
                                            <th class="id">Tipo</th>
                                            <th class="nome">Manobrista</th>
                                            <th class="operacao">Estabelecimento</th>
                                            <th class="tempo">Tempo</th>
                                            <th class="placa">Placa</th>
                                            <th class="saida">Saída</th>
                                            <th class="entrada">Entrada</th>
                                        </tr>`);

        //For to insert all the data on a table
        for (i = 0; i < Object.keys(objeto).length; i++) {
            Object.assign(objeto[i], {"TIME1": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP1)), new Date(Date.parse(objeto[i].TIME2_VP1)))});
            Object.assign(objeto[i], {"TIME2": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP2)), new Date(Date.parse(objeto[i].TIME2_VP2)))});

            addToTable("Estacionar", objeto[i].NAME1, "Inteli", objeto[i].TIME1, objeto[i].PLATE, objeto[i].TIME1_VP1, objeto[i].TIME2_VP1);
            addToTable("Devolver", objeto[i].NAME2, "Inteli", objeto[i].TIME2, objeto[i].PLATE, objeto[i].TIME1_VP2, objeto[i].TIME2_VP2);
        }
    });
});

//Function to convert hour to minutes and calculate the absolute vallet time
function convertToMinutes(time1, time2){
    var firstMin = time1.getHours() * 60;
    var firstMin = firstMin + time1.getMinutes();

    var lastMin = time2.getHours() * 60;
    var lastMin = lastMin + time2.getMinutes();

    var absoluteMinutes = lastMin - firstMin;

    return absoluteMinutes;
}

//Function to add itens to html table
function addToTable(type, name, operation, time, plate, left, join){
    $("#search-table").append(`<tr id="partner-list">
                                    <th class="id">` + type + `</th>
                                    <th class="nome">` + name + `</th>
                                    <th class="operacao">` + operation + `</th>
                                    <th class="tempo">` + time + `</th>
                                    <th class="placa">` + plate + `</th>
                                    <th class="saida">` + left + `</th>
                                    <th class="entrada">` + join + `</th>
                                </tr>`);
}

//Function to check when a key is pressed to realize the search
$(".pesquisa").keyup(function(event) {
    //Checking if key pressed is Enter
    if (event.which === 13) {
        //Switch to check which filter will be applied
        switch ($(".search-type").val()){
            case "Manobrista":
                $.get(url + "/getStatistics", function (resultado) {
                    var objeto = JSON.parse(resultado);

                    //Removing datas that don't match with required name
                    for (i = 0; i < Object.keys(objeto).length; i ++){
                        if (objeto[i].NAME1 != $(".pesquisa").val() && objeto[i].NAME2 != $(".pesquisa").val()){
                            delete objeto[i];
                        }
                    }

                    //Reseting array index
                    objeto = objeto.filter(function(){return true;});

                    //Starting table title
                    $("#search-table").html(`<tr id="partner-list">
                                                        <th class="id">Tipo</th>
                                                        <th class="nome">Manobrista</th>
                                                        <th class="operacao">Estabelecimento</th>
                                                        <th class="tempo">Tempo</th>
                                                        <th class="placa">Placa</th>
                                                        <th class="saida">Saída</th>
                                                        <th class="entrada">Entrada</th>
                                                    </tr>`);

                    //For to insert all the data on a table
                    for (i = 0; i < Object.keys(objeto).length; i++) {
                        Object.assign(objeto[i], {"TIME1": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP1)), new Date(Date.parse(objeto[i].TIME2_VP1)))});
                        Object.assign(objeto[i], {"TIME2": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP2)), new Date(Date.parse(objeto[i].TIME2_VP2)))});

                        if (objeto[i].NAME1 == $(".pesquisa").val()){
                            addToTable("Estacionar", objeto[i].NAME1, "Inteli", objeto[i].TIME1, objeto[i].PLATE, objeto[i].TIME1_VP1, objeto[i].TIME2_VP1);
                        }
                        if (objeto[i].NAME2 == $(".pesquisa").val()){
                            addToTable("Devolver", objeto[i].NAME2, "Inteli", objeto[i].TIME2, objeto[i].PLATE, objeto[i].TIME1_VP2, objeto[i].TIME2_VP2);
                        }
                    }
                });

                break;
            case "Placa":
                $.get(url + "/getStatistics", function (resultado) {
                    var objeto = JSON.parse(resultado);

                    //Removing datas that don't match with required plate
                    for (i = 0; i <= Object.keys(objeto).length; i ++){
                        if (objeto[i].PLATE != $(".pesquisa").val()){
                            delete objeto[i];
                        }
                    }

                    //Reseting array index
                    objeto = objeto.filter(function(){return true;});

                    //Starting table title
                    $("#search-table").html(`<tr id="partner-list">
                                                        <th class="id">Tipo</th>
                                                        <th class="nome">Manobrista</th>
                                                        <th class="operacao">Estabelecimento</th>
                                                        <th class="tempo">Tempo</th>
                                                        <th class="placa">Placa</th>
                                                        <th class="saida">Saída</th>
                                                        <th class="entrada">Entrada</th>
                                                    </tr>`);

                    //For to insert all the data on a table
                    for (i = 0; i <= Object.keys(objeto).length; i++) {
                        Object.assign(objeto[i], {"TIME1": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP1)), new Date(Date.parse(objeto[i].TIME2_VP1)))});
                        Object.assign(objeto[i], {"TIME2": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP2)), new Date(Date.parse(objeto[i].TIME2_VP2)))});

                        addToTable("Estacionar", objeto[i].NAME1, "Inteli", objeto[i].TIME1, objeto[i].PLATE, objeto[i].TIME1_VP1, objeto[i].TIME2_VP1);
                        addToTable("Devolver", objeto[i].NAME2, "Inteli", objeto[i].TIME2, objeto[i].PLATE, objeto[i].TIME1_VP2, objeto[i].TIME2_VP2);
                    }
                });    

                break;
            case "Data":
                $.get(url + "/getStatistics", function (resultado) {
                    var objeto = JSON.parse(resultado);

                    var searchDate = new Date(Date.parse($(".pesquisa").val() + " 10:10:10"));

                    //Removing datas that don't match with required date
                    for (i = 0; i <= Object.keys(objeto).length; i ++){
                        var dateIndex = new Date(Date.parse(objeto[i].TIME1_VP1));

                        if (dateIndex.getFullYear() == searchDate.getFullYear() && dateIndex.getMonth() == searchDate.getMonth() && dateIndex.getDate() == searchDate.getDate()){}
                        else {
                            delete objeto[i];
                        }             
                    }

                    //Reseting array index
                    objeto = objeto.filter(function(){return true;});

                    //Starting table title
                    $("#search-table").html(`<tr id="partner-list">
                                                        <th class="id">Tipo</th>
                                                        <th class="nome">Manobrista</th>
                                                        <th class="operacao">Estabelecimento</th>
                                                        <th class="tempo">Tempo</th>
                                                        <th class="placa">Placa</th>
                                                        <th class="saida">Saída</th>
                                                        <th class="entrada">Entrada</th>
                                                    </tr>`);

                    //For to insert all the data on a table
                    for (i = 0; i < Object.keys(objeto).length; i++) {
                        Object.assign(objeto[i], {"TIME1": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP1)), new Date(Date.parse(objeto[i].TIME2_VP1)))});
                        Object.assign(objeto[i], {"TIME2": convertToMinutes(new Date(Date.parse(objeto[i].TIME1_VP2)), new Date(Date.parse(objeto[i].TIME2_VP2)))});

                        addToTable("Estacionar", objeto[i].NAME1, "Inteli", objeto[i].TIME1, objeto[i].PLATE, objeto[i].TIME1_VP1, objeto[i].TIME2_VP1);
                        addToTable("Devolver", objeto[i].NAME2, "Inteli", objeto[i].TIME2, objeto[i].PLATE, objeto[i].TIME1_VP2, objeto[i].TIME2_VP2);
                    }
                });
                break;
        }
    }
});
