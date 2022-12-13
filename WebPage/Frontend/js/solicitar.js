function sendToTotem(){

    //Post request to add solicited vallet to totem
    $.ajax({
        type: 'POST',
        url: "http://10.128.64.59:2707/addValletToTotem",
        data: {
            Plate: $(".prism").val()
        }
    }).done(function () {
        console.log("enviado com sucesso");
    })
}