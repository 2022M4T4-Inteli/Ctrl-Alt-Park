var actualType = "";

var url = "http://10.128.64.59:2707";

function sendToTotem(){

    if (actualType == "Registrar"){
        //Post request to add solicited vallet to totem
        $.ajax({
            type: 'POST',
            url: url + "/addValletToTotem",
            data: {
                Plate: $(".plate").val()
            }
        }).done(function () {
            console.log("enviado com sucesso");
        })
    }
    else {
        //Post request to update status from solicited vallets
        $.ajax({
            type: 'POST',
            url: url + "/updateSolicitedVallets",
            data: {
                Plate: $(".plate").val(),
                Status: "FALSE"
            }
        }).done(function () {
            console.log("enviado com sucesso");
        })
    }
}

//Function to change html page according of which action user wants to make between register an vallet or solicit
function changeActionType(buttonType) {
    if (buttonType == "Registrar"){
        actualType = "Registrar"

        $('.register').css('background-color', 'rgba(255, 255, 255, 0.985)');
        $('.register').css('color', '#000000');
        $('.solicited').css('background-color', 'rgba(5, 5, 5, 0)');
        $('.solicited').css('color', '#FFFFFF');
        $('.window').css('border-top-left-radius', '0px');
        $('.prism').css('display', 'block');
        $(".requestButton").html('Registrar');
    }
    else {
        $('.solicited').css('background-color', 'rgba(255, 255, 255, 0.985)');
        $('.solicited').css('color', '#000000');
        $('.register').css('background-color', 'rgba(5, 5, 5, 0)');
        $('.register').css('color', '#FFFFFF');
        $('.window').css('border-top-left-radius', '15px');
        $('.prism').css('display', 'none');
        $(".requestButton").html('Solicitar');
    }
}