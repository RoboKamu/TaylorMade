// Fetch and paint all 4 buttons
async function fetchStatus() {
    const res = await fetch("/portstatus");
    const status = await res.json();
    Object.entries(status).forEach(([port, isOn]) => {
      const btn    = document.getElementById(`btn-${port}`);
      const label  = document.getElementById(`status-${port}`);
      label.textContent = isOn ? "ON" : "OFF";
      btn.className   = isOn ? "btn btn-success" : "btn btn-danger";
    });
  }
  
  // Toggle one port, then repaint
  async function togglePort(port) {
    // read current label
    const label = document.getElementById(`status-${port}`).textContent;
    const action = label === "ON" ? "OFF" : "ON";
    const res = await fetch(`/${action}?port=${port}`);
    if (!res.ok) {
      alert("Toggle failed");
      return;
    }
    await fetchStatus();
  }
  
  // set up polling
  document.addEventListener("DOMContentLoaded", () => {
    fetchStatus();
    setInterval(fetchStatus, 3000);
  });
  