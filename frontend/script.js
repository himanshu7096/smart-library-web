
// ==========================================
// SMART LIBRARY MANAGEMENT SYSTEM
// Connected to C++ Backend
// ==========================================

const API_URL = "/api";

let books = [];

// ==========================================
// API COMMUNICATION
// ==========================================

async function apiRequest(endpoint, method = "GET", data = null) {

    const options = {
        method: method,
        headers: {}
    };

    // Send JSON header only when a request has a body.
    // This avoids unnecessary browser preflight requests for GET.
    if (data !== null) {
        options.headers["Content-Type"] = "application/json";
        options.body = JSON.stringify(data);
    }

    const response = await fetch(API_URL + endpoint, options);

    let result;

    try {
        result = await response.json();
    }
    catch {
        throw new Error("Invalid response received from server.");
    }

    if (!response.ok) {
        throw new Error(result.message || "Request failed.");
    }

    return result;
}

// ==========================================
// LOAD BOOKS FROM C++ BACKEND
// ==========================================

async function loadBooks() {

    try {
        books = await apiRequest("/books");
        renderAll();
        return true;
    }
    catch (error) {
        console.error("Backend error:", error);
        notify("Unable to connect to C++ backend!");
        return false;
    }
}

// ==========================================
// NAVIGATION
// ==========================================

function showPage(page) {

    document.querySelectorAll(".page").forEach(section => {
        section.classList.remove("active");
    });

    const selectedPage = document.getElementById(page + "-page");

    if (!selectedPage) {
        console.error("Page not found:", page);
        return;
    }

    selectedPage.classList.add("active");

    document.querySelectorAll(".nav-link").forEach(button => {
        button.classList.toggle(
            "active",
            button.dataset.page === page
        );
    });

    const titles = {
        dashboard: "Dashboard Overview",
        books: "All Books",
        add: "Add New Book",
        issued: "Issued Books"
    };

    document.getElementById("page-title").textContent = titles[page];
    document.getElementById("breadcrumb").textContent = titles[page];

    renderAll();
}

document.querySelectorAll(".nav-link").forEach(button => {

    button.addEventListener("click", () => {
        showPage(button.dataset.page);
    });

});

// ==========================================
// NOTIFICATION
// ==========================================

function notify(message) {

    const toast = document.getElementById("toast");

    toast.textContent = message;
    toast.classList.add("show");

    setTimeout(() => {
        toast.classList.remove("show");
    }, 2500);
}

// ==========================================
// ADD BOOK
// ==========================================

document.getElementById("book-form").addEventListener(
    "submit",
    async function(event) {

        event.preventDefault();

        const id = Number(
            document.getElementById("book-id").value
        );

        const title = document
            .getElementById("book-title")
            .value.trim();

        const author = document
            .getElementById("book-author")
            .value.trim();

        const publisher = document
            .getElementById("book-publisher")
            .value.trim();

        if (!Number.isInteger(id) || id <= 0) {
            notify("Enter a valid positive Book ID.");
            return;
        }

        if (!title) {
            notify("Book title cannot be empty.");
            return;
        }

        try {

            await apiRequest("/books", "POST", {
                id,
                title,
                author,
                publisher
            });

            this.reset();

            const loaded = await loadBooks();

            if (loaded) {
                notify("Book added successfully!");
                showPage("books");
            }

        }
        catch (error) {
            console.error(error);
            notify(error.message);
        }
    }
);

// ==========================================
// BOOK TABLE
// ==========================================

function bookRow(book, showActions = true) {

    const status = book.issued ? "Issued" : "Available";

    const actions = showActions
        ? `
            <button class="action-btn"
                onclick="toggleIssue(${book.id})">
                ${book.issued ? "Return" : "Issue"}
            </button>

            <button class="action-btn danger"
                onclick="deleteBook(${book.id})">
                Delete
            </button>
        `
        : "";

    return `
        <tr>
            <td>
                <strong>${escapeHTML(book.title)}</strong>
                <small>${escapeHTML(book.author)}</small>
            </td>

            <td>#${book.id}</td>

            <td>${escapeHTML(book.publisher)}</td>

            <td>
                <span class="status ${book.issued ? "issued" : "available"}">
                    ${status}
                </span>
            </td>

            ${showActions ? `<td>${actions}</td>` : ""}
        </tr>
    `;
}

// ==========================================
// ESCAPE USER INPUT
// ==========================================

function escapeHTML(value) {

    return String(value).replace(/[&<>"']/g, character => ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#39;"
    })[character]);
}

// ==========================================
// DISPLAY ALL BOOKS
// ==========================================

function renderBooks() {

    const search = document
        .getElementById("search-input")
        .value.toLowerCase();

    const filter = document
        .getElementById("filter-status")
        .value;

    const filtered = books.filter(book => {

        const matchesSearch =
            book.title.toLowerCase().includes(search) ||
            book.author.toLowerCase().includes(search) ||
            String(book.id).includes(search);

        const matchesStatus =
            filter === "all" ||
            (filter === "issued" && book.issued) ||
            (filter === "available" && !book.issued);

        return matchesSearch && matchesStatus;
    });

    document.getElementById("books-table").innerHTML =
        filtered.length
            ? filtered.map(book => bookRow(book)).join("")
            : `<tr><td colspan="5" class="empty">No books found.</td></tr>`;
}

// ==========================================
// DASHBOARD STATISTICS
// ==========================================

function renderDashboard() {

    const total = books.length;

    const issued = books.filter(book => book.issued).length;

    const available = total - issued;

    const percentage = total
        ? Math.round((available / total) * 100)
        : 0;

    document.getElementById("total-books").textContent = total;

    document.getElementById("available-books").textContent = available;

    document.getElementById("issued-books").textContent = issued;

    document.getElementById("availability").textContent =
        percentage + "%";

    const recent = [...books].reverse().slice(0, 5);

    document.getElementById("recent-table").innerHTML =
        recent.length
            ? recent.map(book => bookRow(book, false)).join("")
            : `<tr><td colspan="4" class="empty">No books added yet.</td></tr>`;
}

// ==========================================
// ISSUE AND RETURN BOOK
// ==========================================

async function toggleIssue(id) {

    const book = books.find(book => book.id === id);

    if (!book) {
        notify("Book not found!");
        return;
    }

    const action = book.issued ? "return" : "issue";

    try {

        await apiRequest(`/books/${id}/${action}`, "POST");

        const loaded = await loadBooks();

        if (loaded) {
            notify(
                action === "issue"
                    ? "Book issued successfully!"
                    : "Book returned successfully!"
            );
        }

    }
    catch (error) {
        console.error(error);
        notify(error.message);
    }
}

// ==========================================
// DELETE BOOK
// ==========================================

async function deleteBook(id) {

    const confirmed = confirm(
        "Are you sure you want to delete this book?"
    );

    if (!confirmed) return;

    try {

        await apiRequest(`/books/${id}`, "DELETE");

        const loaded = await loadBooks();

        if (loaded) {
            notify("Book deleted successfully!");
        }

    }
    catch (error) {
        console.error(error);
        notify(error.message);
    }
}

// ==========================================
// ISSUED BOOKS
// ==========================================

function renderIssued() {

    const issued = books.filter(book => book.issued);

    document.getElementById("issued-table").innerHTML =
        issued.length
            ? issued.map(book => `
                <tr>
                    <td>
                        <strong>${escapeHTML(book.title)}</strong>
                        <small>${escapeHTML(book.author)}</small>
                    </td>

                    <td>#${book.id}</td>

                    <td>${escapeHTML(book.publisher)}</td>

                    <td>
                        <button class="action-btn"
                            onclick="toggleIssue(${book.id})">
                            Return Book
                        </button>
                    </td>
                </tr>
            `).join("")
            : `<tr><td colspan="4" class="empty">No issued books.</td></tr>`;
}

// ==========================================
// RENDER EVERYTHING
// ==========================================

function renderAll() {

    renderDashboard();
    renderBooks();
    renderIssued();
}   

// ==========================================
// SEARCH AND FILTER
// ==========================================

document.getElementById("search-input")
    .addEventListener("input", renderBooks);

document.getElementById("filter-status")
    .addEventListener("change", renderBooks);

// ==========================================
// INITIALIZE APPLICATION
// ==========================================

loadBooks();
