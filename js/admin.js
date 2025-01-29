function updateDevices(state) {
  const content = document.getElementById("content");

  // Verificar si el estado está definido y es válido (1 o 0)
  if (state === undefined || (state !== 0 && state !== 1)) {
    return; // Salir de la función si el estado no es válido
  }

  // Enviar 1 para "Encendido" y 0 para "Apagado"
  fetch("php/update_devices.php", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ state: state }), // state = 1 o 0
  })
    .then((response) => response.json())
    .then((data) => {
      console.log(data.message); // Imprime el mensaje recibido del servidor

      // Verificar si la respuesta indica éxito
      if (data.message === "Estado actualizado correctamente.") {
        // Mostrar mensaje de éxito
        Swal.fire({
          icon: "success",
          title: "Estado actualizado",
          text: data.message,
          confirmButtonText: "Aceptar",
        }).then((result) => {
          if (result.isConfirmed) {
            // Redirigir a admin.html si la respuesta es exitosa
            fetch("php/show_tables.php?table=dispositivos")
              .then((response) => {
                if (!response.ok) throw new Error("Error en la solicitud.");
                return response.text();
              })
              .then((html) => {
                console.log("HTML recibido:", html); // Para depuración
                content.innerHTML = html;
              })
              .catch((error) => {
                console.error(
                  "Error al recargar la tabla de dispositivos:",
                  error
                );
                Swal.fire({
                  icon: "error",
                  title: "Error",
                  text: "Hubo un problema al recargar la tabla de dispositivos.",
                  confirmButtonText: "Aceptar",
                });
              });
          }
        });
      } else {
        // Si hay un error, mostrar mensaje de error
        Swal.fire({
          icon: "error",
          title: "Error",
          text: data.message,
          confirmButtonText: "Aceptar",
        });
      }
    })
    .catch((error) => {
      console.error("Error:", error);

      // En caso de error en la solicitud, mostrar mensaje de error
      Swal.fire({
        icon: "error",
        title: "Error al actualizar el estado",
        text: "Hubo un problema al intentar actualizar el estado. Inténtalo de nuevo.",
        confirmButtonText: "Aceptar",
      });
    });
}

function toggleDevice(deviceId, currentState) {
  // Determinar el nuevo estado
  const newState = currentState === 1 ? 0 : 1;
  const content = document.getElementById("content");

  // Enviar la solicitud al servidor
  fetch("php/control_device.php", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ id: deviceId, state: newState }), // id del dispositivo y nuevo estado
  })
    .then((response) => response.json())
    .then((data) => {
      // Verificar el estado de la respuesta
      if (data.success) {
        Swal.fire({
          icon: "success",
          title: "Estado actualizado",
          text: data.message,
          confirmButtonText: "Aceptar",
        }).then(() => {
          // Recargar la página para reflejar los cambios
          fetch("php/show_tables.php?table=dispositivos")
            .then((response) => {
              if (!response.ok) throw new Error("Error en la solicitud.");
              return response.text();
            })
            .then((html) => {
              console.log("HTML recibido:", html); // Para depuración
              content.innerHTML = html;
            })
            .catch((error) => {
              console.error(
                "Error al recargar la tabla de dispositivos:",
                error
              );
              Swal.fire({
                icon: "error",
                title: "Error",
                text: "Hubo un problema al recargar la tabla de dispositivos.",
                confirmButtonText: "Aceptar",
              });
            });
        });
      } else {
        Swal.fire({
          icon: "error",
          title: "Error",
          text: data.message,
          confirmButtonText: "Aceptar",
        });
      }
    })
    .catch((error) => {
      console.error("Error:", error);
      Swal.fire({
        icon: "error",
        title: "Error al actualizar",
        text: "Hubo un problema al intentar actualizar el estado del dispositivo.",
        confirmButtonText: "Aceptar",
      });
    });
}
