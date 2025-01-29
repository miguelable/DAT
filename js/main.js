// document.addEventListener("DOMContentLoaded", function () {
//   initializeSidebar();
//   initializeFetchData();
//   initializeFormValidation();
// });

document.addEventListener("DOMContentLoaded", function () {
  if (typeof initializeSidebar === "function") {
    initializeSidebar();
  }
  if (typeof initializeFormValidation === "function") {
    initializeFormValidation();
  }
  if (typeof initializeFetchData === "function") {
    initializeFetchData();
  }
  if (typeof updateDevices === "function") {
    updateDevices();
  }
});
