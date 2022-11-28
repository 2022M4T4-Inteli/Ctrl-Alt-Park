setInterval(function(){ 
    console.log("Teste");

    $("#arrived").html(`<div class="label-title default-border" style="--color: #70D44B;">Chegou</div>`);
    $("#on-course").html(`<div class="label-title default-border" style="--color: #F37F35;">A caminho</div>`);

    $.get("http://10.128.64.121:2707/getAllVallets", function (resultado) {
    //$.get("http://127.0.0.1:2707/getAllVallets", function (resultado) {
        var objeto = JSON.parse(resultado);
        var aux = 1;
        for (i = 0; i < Object.keys(objeto).length; i++) {
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
                                                ` + objeto[i].TIME + `
                                            </div>
                                        </div>
                                    </div>`)
            }
        }
    });
}, 1000);