var url = "http://10.128.64.59:2707";

//Function that execute every 100 milliseconds
setInterval(async function(){

    //Request to get all actives vallets to include on totem
    $.get(url + "/getAllVallets", function (resultado) {
        var objeto = JSON.parse(resultado);

        //Request to get all solicited vallets to include on totem
        $.get(url + "/getAllSolicited", function (resultado) {
            var objetoSolicitados = JSON.parse(resultado);

            //Starting totems titles
            $("#requested").html(`<div class="label-title default-border" style="--color: #FF4848;">Solicitou</div>`);
            $("#arrived").html(`<div class="label-title default-border" style="--color: #70D44B;">Chegou</div>`);
            $("#on-course").html(`<div class="label-title default-border" style="--color: #F37F35;">A caminho</div>`);

            //For to add all the vallet on totem
            for (i = 0; i < Object.keys(objeto).length; i++) {

                //If to check which collumn has to be include the vallet, between fineshed and on course
                if (objeto[i].STATUS == "Carro devolvido"){
                    $("#arrived").append(`<div class="plate default-border shadow">
                                                <div class="plate-image">
                                                    <img src="../icons/car-plate.png">
                                                    <h2 class="plate-text">` + objeto[i].PLATE + `</h2>
                                                </div>
                                                <div class="plate-content">
                                                    <div class="plate-status default-border" style="--color: #70D44B;">
                                                        <img src="../icons/check_icon.png">
                                                    </div>
                                                </div>
                                            </div>`)
                }
                else{
                    $("#on-course").append(`<div class="plate default-border shadow">
                                            <div class="plate-image">
                                                <img src="../icons/car-plate.png">
                                                <h2 class="plate-text">` + objeto[i].PLATE + `</h2>
                                            </div>
                                            <div class="plate-content">
                                                <div class="plate-status default-border" style="--color: #F37F35;">
                                                    ` + parseFloat(objeto[i].TIME).toFixed(2) + ` min 
                                                </div>
                                            </div>
                                        </div>`)
                }
            }
            
            //For to check if solicited vallet are actives
            for (j = 0; j < Object.keys(objetoSolicitados).length; j ++) {            
                for (i = 0; i < Object.keys(objeto).length; i++) {
                    if (objeto[i].STATUS == "Carro devolvido"){
                        continue;
                    }

                    if (objetoSolicitados[j].PLATE == objeto[i].PLATE){
                        //Post request to update status from solicited vallets
                        $.ajax({
                            type: 'POST',
                            url: url + "/updateSolicitedVallets",
                            data: {
                                Plate: objetoSolicitados[j].PLATE,
                                Status: "TRUE"
                            }
                        }).done(function () {
                            console.log("enviado com sucesso");
                        })

                        delete objetoSolicitados[i];
                    }
                }
            }

            objetoSolicitados = objetoSolicitados.filter(function(){return true;});

            //For to add all solicited vallets on totem
            for (i = 0; i < Object.keys(objetoSolicitados).length; i ++) {
                $("#requested").append(`<div class="plate default-border shadow">
                                            <div class="plate-image">
                                                <img src="../icons/car-plate.png">
                                                <h2 class="plate-text">` + objetoSolicitados[i].PLATE + `</h2>
                                            </div>
                                            <div class="plate-content">
                                                <div class="plate-status default-border" style="--color: #FF4848;">
                                                </div>
                                            </div>
                                        </div>`);
            }
        });
    });
}, 100);