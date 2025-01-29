function initializeFormValidation() {
  // comprobar si submit-btn existe en la página
  const submitBtn = document.getElementById("submit-btn");
  if (!submitBtn) {
    console.log("No se encontró el botón de envío del formulario.");
    return;
  }

  // Deshabilitar el botón de submit y cambiar su color a gris al inicio
  submitBtn.disabled = true;
  submitBtn.style.backgroundColor = "gray";

  console.log("Inicializando validación de formulario...");
  console.log(userData);
  // obtener el elemento submit-btn
  submitBtn.addEventListener("click", function () {
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
      alert("Por favor, ingresa un número válido de entradas.");
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
          <input type="text" id="name-${i}" name="name-${i}" value="${
        userData.username || ""
      }" required>

          <label for="email-${i}">Correo Electrónico:</label>
          <input type="email" id="email-${i}" name="email-${i}" value="${
        userData.email || ""
      }" required>

          <label for="altura-${i}">Altura (en metros):</label>
          <input type="number" id="altura-${i}" name="altura-${i}" value="${
        userData.altura || ""
      }" required>

          <label for="edad-${i}">Edad:</label>
          <input type="number" id="edad-${i}" name="edad-${i}" value="${
        userData.edad || ""
      }" required>

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
      formData.append(
        "ticketType",
        document.getElementById("ticket-type").value
      );
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
            icon: "warning",
            title: "Faltan datos",
            text: `Por favor, completa todos los campos del visitante ${i}.`,
            confirmButtonText: "Entendido",
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
      fetch("php/procesar_compra.php", {
        method: "POST",
        body: formData,
      })
        .then((response) => response.json())
        .then((data) => {
          console.log("Respuesta del servidor:", data); // Verifica el contenido en la consola

          if (data.status === "success") {
            Swal.fire({
              icon: "success",
              title: "Compra realizada con éxito",
              text: data.message,
            }).then(() => (location.href = "codigo.html"));
          } else {
            Swal.fire({
              icon: "error",
              title: "Error",
              text: data.message,
            });
          }
        })
        .catch((error) => {
          console.error("Error en la petición:", error); // Manejo de errores de red
          Swal.fire({
            icon: "error",
            title: "Error",
            text: "Hubo un problema al procesar la compra.",
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
}
