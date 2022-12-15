var url = "http://10.128.64.59:2707";

//Function to check if credentials are in the database
function validateLogin(){

    $.get(url + "/getLogin/" + $(".username").val() + "/" + $(".password").val(), function (resultado) {
        var objeto = JSON.parse(resultado);

        if (Object.keys(objeto).length == 0){}
        else{
            window.location = url + "/produtividade.html";
        }
    });
}