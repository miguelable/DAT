function initializeSidebar() {
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
          const response = await fetch(
            `php/show_tables.php?table=${tableName}`
          );
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
}
