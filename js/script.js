document.addEventListener("DOMContentLoaded", function () {
  const adminSidebar = document.querySelector(".sidebar-admin");
  const content = document.getElementById("content");

  // Asegúrate de que el sidebar de invitado esté bien definido
  const guestSidebar = document.querySelector(".sidebar");

  // Comportamiento para el sidebar de invitados
  if (guestSidebar) {
    const links = guestSidebar.querySelectorAll("a");
    links.forEach((link) => {
      link.addEventListener("click", function () {
        // Cambiar la clase activa
        links.forEach((link) => link.classList.remove("active"));
        this.classList.add("active"); // Cambia la clase activa solo al enlace clicado
      });
    });
  }

  // Comportamiento para el sidebar de administradores
  if (adminSidebar) {
    const links = adminSidebar.querySelectorAll("a");
    links.forEach((link) => {
      link.addEventListener("click", async function (event) {
        event.preventDefault(); // Evitar la navegación predeterminada
        // Cambiar la clase activa
        links.forEach((link) => link.classList.remove("active"));
        this.classList.add("active"); // Cambiar la clase activa al enlace clicado
        // Obtener el nombre de la tabla desde el atributo data-table
        const tableName = this.getAttribute("data-table");
        console.log("Tabla seleccionada:", tableName); // Log para ver qué tabla se ha seleccionado
        try {
          // Realizar la solicitud fetch a show_tables.php
          const response = await fetch(`show_tables.php?table=${tableName}`);
          console.log("Respuesta recibida:", response); // Verificar respuesta
          if (!response.ok) throw new Error("Error en la solicitud.");
          const html = await response.text();
          console.log("HTML recibido:", html); // Verificar el HTML recibido
          // Insertar la tabla HTML en el contenido
          content.innerHTML = html;
        } catch (error) {
          console.error("Error al cargar la tabla:", error); // Ver el error si ocurre
          content.innerHTML = `<p>Error al cargar los datos: ${error.message}</p>`;
        }
      });
    });
  }

  // Verificar si estamos en la página 'codigo.html'
  if (window.location.pathname.endsWith("codigo.html")) {
    console.log("Estamos en codigo.html, se realizará la solicitud fetch");
    // Solo hacer la solicitud fetch si estamos en 'codigo.html'
    fetch("get_user_data.php")
      .then((response) => {
        console.log("Respuesta del fetch:", response); // Verificar si la respuesta fue exitosa
        return response.json();
      })
      .then((data) => {
        console.log("Datos recibidos:", data); // Ver los datos recibidos
        if (data.error) {
          document.getElementById("user-info").innerHTML = `
                        <div style="text-align: center;">
                            <!-- Registro -->
                            <h2 style="font-size: 24px; margin-bottom: 10px;">
                                ¿No tienes cuenta?
                            </h2>
                            <p style="font-size: 18px; margin-bottom: 20px;">
                                Regístrate ahora para disfrutar de información exclusiva y muchas sorpresas.
                            </p>
                            <p>
                                <a href="register.html" class="btn btn-register">Registrarse</a>
                            </p>
                
                            <!-- Inicio de sesión -->
                            <h2 style="font-size: 24px; margin-top: 30px; margin-bottom: 10px;">
                                ¿Ya tienes una cuenta?
                            </h2>
                            <p style="font-size: 18px; margin-bottom: 20px;">
                                Inicia sesión para acceder a tu zona personal y continuar la experiencia.
                            </p>
                            <p>
                                <a href="login.html" class="btn btn-login">Iniciar sesión</a>
                            </p>
                
                            <!-- Compra de entradas -->
                            <h2 style="font-size: 24px; margin-top: 30px; margin-bottom: 10px;">
                                ¡Atrévete a más!
                            </h2>
                            <p style="font-size: 18px; margin-bottom: 20px;">
                                Compra tus entradas para el parque y vive una aventura inolvidable.
                            </p>
                            <p>
                                <a href="compraentradas.html" class="btn btn-buy">¡Compra tus entradas ahora!</a>
                            </p>
                        </div>
                    `;
        } else {
          const user = data.user || {};
          const parques = data.parques || {};
          const unidadFamiliar = data.unidad_familiar || {};

          const safeValue = (value) =>
            value !== null && value !== undefined ? value : "";

          let htmlContent = `
                          <h2>Bienvenid@, ${safeValue(user.username)}</h2>
                          <h3><strong>Datos Personales:</strong></h3>
                          <ul>
                      `;

          if (user.email) {
            htmlContent += `<li><strong>Correo:</strong> ${safeValue(
              user.email
            )}</li>`;
          }
          if (user.edad) {
            htmlContent += `<li><strong>Edad:</strong> ${safeValue(
              user.edad
            )}</li>`;
          }
          if (user.altura) {
            htmlContent += `<li><strong>Altura:</strong> ${safeValue(
              user.altura
            )}</li>`;
          }
          if (user.fastpass) {
            htmlContent += `<li><strong>FastPass:</strong> ${safeValue(
              user.fastpass
            )}</li>`;
          }

          htmlContent += `</ul><h3>Parques:</h3><ul>`;

          if (Object.keys(parques).length === 0) {
            htmlContent += `<li>No tiene ningún parque asociado</li>`;
            // Si no hay parques asociados, preguntar si desea añadir uno
            htmlContent += `
                              <p style="margin-top: 10px;">
                                  <a href="compraentradas.html" class="btn btn-buy">Comprar entradas</a>
                              </p>
                          `;
          } else {
            for (let parque in parques) {
              htmlContent += `
                                  <li>
                                      <strong>${parque}:</strong> 
                                      Fecha de acceso: ${safeValue(
                                        parques[parque].fecha_acceso
                                      )} | 
                                      Fecha de salida: ${safeValue(
                                        parques[parque].fecha_salida
                                      )}
                                  </li>
                              `;
            }
          }

          htmlContent += `</ul><h3>Unidad Familiar:</h3><ul>`;

          if (unidadFamiliar === "No pertenece a ninguna unidad familiar") {
            htmlContent += `<li>${unidadFamiliar}</li>`;
          } else {
            if (unidadFamiliar.responsable_1) {
              htmlContent += `<li><strong>Responsable: </strong>${safeValue(
                unidadFamiliar.responsable_1
              )}</li>`;
            }
            if (unidadFamiliar.responsable_2) {
              htmlContent += `<li><strong>Responsable: </strong>${safeValue(
                unidadFamiliar.responsable_2
              )}</li>`;
            }
            if (unidadFamiliar.menor_1) {
              htmlContent += `<li><strong>Menor: </strong>${safeValue(
                unidadFamiliar.menor_1
              )}</li>`;
            }
            if (unidadFamiliar.menor_2) {
              htmlContent += `<li><strong>Menor: </strong>${safeValue(
                unidadFamiliar.menor_2
              )}</li>`;
            }
            if (unidadFamiliar.menor_3) {
              htmlContent += `<li><strong>Menor: </strong>${safeValue(
                unidadFamiliar.menor_3
              )}</li>`;
            }
          }

          htmlContent += `</ul>`;

          document.getElementById("user-info").innerHTML = htmlContent;
        }
      })
      .catch((error) => {
        console.error("Error al cargar los datos del usuario:", error);
        document.getElementById("user-info").innerHTML =
          "<p>Hubo un error al cargar los datos.</p>";
      });
  }
});


document.getElementById("submit-btn").addEventListener("click", function () {
  // Obtener el número de entradas ingresado
  const ticketQuantity = parseInt(
    document.getElementById("ticket-quantity").value
  );
  const ticketType = document.getElementById("ticket-type").value;
  const accessDate = document.getElementById("access-date-1").value;
  const exitDate = document.getElementById("exit-date-1").value;
  const fastpass = document.getElementById("fastpass-1").value;

  // Verificar que el número de entradas es válido
  if (isNaN(ticketQuantity) || ticketQuantity < 1) {
    Swal.fire({
      icon: 'error', // Icono de error
      title: 'Número de entradas no válido',
      text: 'Por favor, ingresa un número válido de entradas.',
      confirmButtonText: 'Entendido', // Texto del botón de confirmación
    });
    return;
  }

  // Ocultar la sección de información de entradas
  document.getElementById("ticket-info").style.display = "none";

  // Mostrar la sección de información de los visitantes
  const visitorsContainer = document.getElementById("visitors-container");
  visitorsContainer.innerHTML = ""; // Limpiar cualquier formulario previo

  // Crear un formulario para cada visitante
  for (let i = 1; i <= ticketQuantity; i++) {
    const visitorForm = `
              <div class="visitor-form">
                  <h4>Visitante ${i}</h4>
                  <label for="name-${i}">Nombre Completo:</label>
                  <input type="text" id="name-${i}" name="name-${i}" required>
  
                  <label for="email-${i}">Correo Electrónico:</label>
                  <input type="email" id="email-${i}" name="email-${i}" required>
  
                  <label for="altura-${i}">Altura (en metros):</label>
                  <input type="number" id="altura-${i}" name="altura-${i}" required>
  
                  <label for="edad-${i}">Edad:</label>
                  <input type="number" id="edad-${i}" name="edad-${i}" required>
  
                  <div style="display: flex; align-items: center; gap: 10px;">
                      <label for="create-account-${i}" style="white-space: nowrap;">¿Deseas crear una cuenta para este visitante?</label>
                      <input type="checkbox" id="create-account-${i}" name="create-account-${i}">
                  </div>
              </div>
          `;
    // Insertar el formulario del visitante en el contenedor
    visitorsContainer.insertAdjacentHTML("beforeend", visitorForm);
  }

  // Mostrar la sección de visitantes
  document.getElementById("visitor-info").style.display = "block";
  validateForm(); // Validar el formulario al mostrarlo
});

document
  .getElementById("finalizar-compra")
  .addEventListener("click", function () {
    // Obtener el número de entradas ingresado para validar los visitantes
    const ticketQuantity = parseInt(
      document.getElementById("ticket-quantity").value
    );

    const visitorsData = [];
    const formData = new FormData();

    // Añadir datos generales al formData
    formData.append("ticketQuantity", ticketQuantity);
    formData.append("ticketType", document.getElementById("ticket-type").value);
    formData.append(
      "accessDate",
      document.getElementById("access-date-1").value
    );
    formData.append("exitDate", document.getElementById("exit-date-1").value);
    formData.append("fastpass", document.getElementById("fastpass-1").value);

    // Verificar que todos los campos de los visitantes estén completos
    for (let i = 1; i <= ticketQuantity; i++) {
      const name = document.getElementById(`name-${i}`).value.trim();
      const email = document.getElementById(`email-${i}`).value.trim();
      const altura = document.getElementById(`altura-${i}`).value.trim();
      const edad = document.getElementById(`edad-${i}`).value.trim();
      const cuenta = document.getElementById(`create-account-${i}`).checked
        ? "on"
        : "off";

      // Validar los campos
      if (!name || !email || !altura || !edad) {
        Swal.fire({
          icon: 'warning',
          title: 'Faltan datos',
          text: `Por favor, completa todos los campos del visitante ${i}.`,
          confirmButtonText: 'Entendido',
        });
        return;
      }

      // Añadir los datos del visitante a un array
      visitorsData.push({ name, email, altura, edad, cuenta });

      // Añadir los datos del visitante al formData
      formData.append(`visitor[${i - 1}][name]`, name);
      formData.append(`visitor[${i - 1}][email]`, email);
      formData.append(`visitor[${i - 1}][altura]`, altura);
      formData.append(`visitor[${i - 1}][edad]`, edad);
      formData.append(`visitor[${i - 1}][create-account]`, cuenta);
    }

    // Enviar los datos al backend usando Fetch API
    fetch('php/procesar_compra.php', {
      method: 'POST',
      body: formData,
    })
      .then((response) => response.json())
      .then((data) => {
        console.log('Respuesta del servidor:', data); // Verifica el contenido en la consola
    
        if (data.status === 'success') {
          Swal.fire({
            icon: 'success',
            title: 'Compra realizada con éxito',
            text: data.message,
          }).then(() => location.reload());
        } else {
          Swal.fire({
            icon: 'error',
            title: 'Error',
            text: data.message,
          });
        }
      })
      .catch((error) => {
        console.error('Error en la petición:', error); // Manejo de errores de red
        Swal.fire({
          icon: 'error',
          title: 'Error',
          text: 'Hubo un problema al procesar la compra.',
        });
      });
});

// Validar que la fecha de salida sea mayor que la fecha de acceso
document
  .getElementById("access-date-1")
  .addEventListener("change", validateDates);
document
  .getElementById("exit-date-1")
  .addEventListener("change", validateDates);
document
  .getElementById("ticket-quantity")
  .addEventListener("input", validateForm);

function validateDates() {
  const accessDate = new Date(document.getElementById("access-date-1").value);
  const exitDate = new Date(document.getElementById("exit-date-1").value);
  const exitDateInput = document.getElementById("exit-date-1");

  if (exitDate <= accessDate) {
    exitDateInput.style.borderColor = "red";
  } else {
    exitDateInput.style.borderColor = ""; // Restablecer el color del borde
  }
  validateForm(); // Validar el formulario después de cambiar las fechas
}

function validateForm() {
  const accessDate = new Date(document.getElementById("access-date-1").value);
  const exitDate = new Date(document.getElementById("exit-date-1").value);
  const ticketQuantity = parseInt(
    document.getElementById("ticket-quantity").value
  );

  let isValid = true;

  if (
    !accessDate ||
    !exitDate ||
    isNaN(ticketQuantity) ||
    ticketQuantity < 1 ||
    exitDate < accessDate
  ) {
    isValid = false;
  }

  const submitBtn = document.getElementById("submit-btn");
  if (isValid) {
    submitBtn.disabled = false;
    submitBtn.style.backgroundColor = ""; // Restablecer el color del botón
  } else {
    submitBtn.disabled = true;
    submitBtn.style.backgroundColor = "gray"; // Cambiar el color del botón a gris
  }
}

function isValidEmail(email) {
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  return emailRegex.test(email);
}

// Llamar a la función antes de permitir el envío
document.addEventListener("input", (event) => {
  if (event.target.type === "email") {
    const email = event.target.value;
    if (!isValidEmail(email)) {
      event.target.style.borderColor = "red";
    } else {
      event.target.style.borderColor = ""; // Resetear estilo
    }
  }
});

function updateDevices(state) {
  // Enviar 1 para "Encendido" y 0 para "Apagado"
  fetch("php/update_devices.php", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ state: state }), // state = 1 o 0
  })
    .then((response) => response.json())
    .then((data) => {
      console.log(data.message); // Imprime el mensaje recibido del servidor
      // Puedes agregar lógica aquí para mostrar un mensaje al usuario
    })
    .catch((error) => {
      console.error("Error:", error);
    });
}
