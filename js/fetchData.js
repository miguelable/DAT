let userData = {}; // Variable global para almacenar los datos del usuario

function initializeFetchData() {
  // Verificar si estamos en la página 'codigo.html'
  console.log("Estamos en codigo.html, se realizará la solicitud fetch");
  // Solo hacer la solicitud fetch si estamos en 'codigo.html'
  fetch("php/get_user_data.php")
    .then((response) => {
      console.log("Respuesta del fetch:", response); // Verificar si la respuesta fue exitosa
      return response.json();
    })
    .then((data) => {
      console.log("Datos recibidos:", data); // Ver los datos recibidos
      userData = data.user || {}; // Almacenar los datos del usuario en la variable global
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
        const pulsera = data.pulsera || {};

        const safeValue = (value) =>
          value !== null && value !== undefined ? value : "";

        // Crear contenido para la columna izquierda
        let leftColumnContent = `
          <h2>Bienvenid@, ${safeValue(user.username)}</h2>
          <h3><strong>Datos Personales:</strong></h3>
          <ul>`;

        if (user.email) {
          leftColumnContent += `<li><strong>Correo:</strong> ${safeValue(
            user.email
          )}</li>`;
        }
        if (user.edad) {
          leftColumnContent += `<li><strong>Edad:</strong> ${safeValue(
            user.edad
          )}</li>`;
        }
        if (user.altura) {
          leftColumnContent += `<li><strong>Altura:</strong> ${safeValue(
            user.altura
          )}</li>`;
        }
        if (user.fastpass) {
          leftColumnContent += `<li><strong>FastPass:</strong> ${safeValue(
            user.fastpass
          )}</li>`;
        }

        leftColumnContent += `</ul><h3>Parques:</h3><ul>`;

        if (Object.keys(parques).length === 0) {
          leftColumnContent += `<li>No tiene ningún parque asociado</li>`;
        } else {
          for (let parque in parques) {
            leftColumnContent += `
              <li>
                <strong>${parque}:</strong> <br>
                Fecha de acceso: ${safeValue(parques[parque].fecha_acceso)} <br>
                Fecha de salida: ${safeValue(parques[parque].fecha_salida)} <br>
              </li>`;
          }
        }

        leftColumnContent += `<p style="margin-top: 10px;">
            <a href="compraentradas.html" class="btn btn-buy">Comprar entradas</a>
         </p>`;

        leftColumnContent += `</ul><h3>Unidad Familiar:</h3><ul>`;

        if (unidadFamiliar === "No pertenece a ninguna unidad familiar") {
          leftColumnContent += `<li>${unidadFamiliar}</li>`;
        } else {
          if (unidadFamiliar.responsable_1) {
            leftColumnContent += `<li><strong>Responsable: </strong>${safeValue(
              unidadFamiliar.responsable_1
            )}</li>`;
          }
          if (unidadFamiliar.responsable_2) {
            leftColumnContent += `<li><strong>Responsable: </strong>${safeValue(
              unidadFamiliar.responsable_2
            )}</li>`;
          }
          // Añadir menores
          ["menor_1", "menor_2", "menor_3"].forEach((menor) => {
            if (unidadFamiliar[menor]) {
              leftColumnContent += `<li><strong>Menor: </strong>${safeValue(
                unidadFamiliar[menor]
              )}</li>`;
            }
          });
        }

        leftColumnContent += `</ul>`;

        // Crear contenido para la columna derecha (pulsera)
        let rightColumnContent = `<h3>Datos de la Pulsera:</h3><ul>`;

        if (pulsera.numero_pulsera) {
          rightColumnContent += `<li><strong>Número de Pulsera:</strong> ${safeValue(
            pulsera.numero_pulsera
          )}</li>`;
        } else {
          rightColumnContent += `<li>No tienes pulsera asociada</li>`;
        }
        if (pulsera.ultimo_registro) {
          rightColumnContent += `<li><strong>Último Registro:</strong> ${safeValue(
            pulsera.ultimo_registro
          )}</li>`;
        }
        if (pulsera.ultima_imagen) {
          rightColumnContent += `
            <li>
              <strong>Última Imagen Registrada:</strong><br>
              <img src="data:image/jpeg;base64,${safeValue(
                pulsera.ultima_imagen
              )}" alt="Última Imagen" style="max-width: 100%; height: auto; margin-top: 10px;">
            </li>`;
        }

        rightColumnContent += `</ul>`;

        // Actualizar HTML con ambas columnas
        document.getElementById("user-info").innerHTML = `
          <div id="flex-container" class="responsive-flex-container">
            <div style="flex: 0.3;">${leftColumnContent}</div>
            <div class="right-column" style="flex: 1;">${rightColumnContent}</div>
          </div>`;
      }
    })
    .catch((error) => {
      console.error("Error al cargar los datos del usuario:", error);
      document.getElementById("user-info").innerHTML =
        "<p>Hubo un error al cargar los datos.</p>";
    });
}
