function cargarDatosPotencia() {
    const xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status === 200) {
            document.getElementById('tabla-container-potencia').innerHTML = this.responseText;
            document.getElementById('tabla-container-led').style.display = 'none';
            document.getElementById('tabla-container-potencia').style.display = 'block';
        } else {
            Swal.fire({
                icon: 'error',
                title: 'Error',
                text: 'Error al cargar los datos'
            });
        }
    };
    xhr.open('GET', 'php/show_data.php', true);
    xhr.send();
}

function cargarDatosLed() {
    const xhr = new XMLHttpRequest();
    xhr.onload = function() {
        document.getElementById('tabla-container-led').innerHTML = this.responseText;
        if (this.status === 200) {
            document.getElementById('tabla-container-potencia').style.display = 'none';
            document.getElementById('tabla-container-led').style.display = 'block';

            document.querySelectorAll('.desired-status').forEach(function(select) {
                select.addEventListener('change', function() {
                    const idSonda = this.getAttribute('data-id');
                    const desiredStatus = this.value;

                    const xhr = new XMLHttpRequest();
                    xhr.open('POST', 'php/update_led_status.php', true);
                    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
                    xhr.onload = function() {
                        if (this.status === 200) {
                            Swal.fire({
                                icon: 'success',
                                title: 'Éxito',
                                text: 'Estado actualizado correctamente'
                            });
                        } else {
                            Swal.fire({
                                icon: 'error',
                                title: 'Error',
                                text: 'Error al actualizar el estado'
                            });
                        }
                    };
                    xhr.send('id_sonda=' + idSonda + '&desired_status=' + desiredStatus + '&source=user');
                });
            });
        } else {
            Swal.fire({
                icon: 'error',
                title: 'Error',
                text: 'Error al cargar los datos'
            });
        }
    };
    xhr.open('GET', 'php/show_leds_data.php', true);
    xhr.send();
}