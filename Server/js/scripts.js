function eliminarDatos() {
    Swal.fire({
        title: '¿Estás seguro?',
        text: "¡No podrás revertir esto!",
        icon: 'warning',
        showCancelButton: true,
        confirmButtonColor: '#3085d6',
        cancelButtonColor: '#d33',
        confirmButtonText: 'Sí, eliminarlo'
    }).then((result) => {
        if (result.isConfirmed) {
            let url = '';
            if (document.getElementById('tabla-container-potencia').style.display === 'block') {
                url = 'php/delete_pot_sensor.php';
            } else if (document.getElementById('tabla-container-led').style.display === 'block') {
                url = 'php/delete_led_status.php';
            }

            if (url) {
                const xhr = new XMLHttpRequest();
                xhr.onload = function() {
                    if (this.status === 200) {
                        Swal.fire(
                            'Eliminado',
                            'Todos los datos han sido eliminados.',
                            'success'
                        );
                        if (url === 'php/delete_pot_sensor.php') {
                            cargarDatosPotencia(); // Recargar los datos después de eliminar
                        } else if (url === 'php/delete_led_status.php') {
                            cargarDatosLed(); // Recargar los datos después de eliminar
                        }
                    } else {
                        Swal.fire({
                            icon: 'error',
                            title: 'Error',
                            text: 'Error al eliminar los datos'
                        });
                    }
                };
                xhr.open('POST', url, true);
                xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
                xhr.send();
            }
        }
    });
}

function cargarDatosPotencia(fromButton = false) {
    const xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status === 200) {
            document.getElementById('tabla-container-potencia').innerHTML = this.responseText;
            if (fromButton) {
                document.getElementById('tabla-container-led').style.display = 'none';
                document.getElementById('tabla-container-potencia').style.display = 'block';
            }
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

function cargarDatosLed(fromButton = false) {
    const xhr = new XMLHttpRequest();
    xhr.onload = function() {
        if (this.status === 200) {
            document.getElementById('tabla-container-led').innerHTML = this.responseText;
            if (fromButton) {
                document.getElementById('tabla-container-potencia').style.display = 'none';
                document.getElementById('tabla-container-led').style.display = 'block';
            }
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

// Llamar a la función cargarDatosLed cada 5 segundos
setInterval(cargarDatosLed, 5000);
// Llamar a la función cargarDatosPotencia cada 5 segundos
setInterval(cargarDatosPotencia, 5000);

// Llamar a la función cargarDatosLed inicialmente para cargar los datos al cargar la página
cargarDatosLed();

// Llamar a la función cargarDatosPotencia inicialmente para cargar los datos al cargar la página
cargarDatosPotencia();