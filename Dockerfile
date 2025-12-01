FROM python:3.11-slim

WORKDIR /app

# System deps for building the C binary
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
 && rm -rf /var/lib/apt/lists/*

# Install API deps
COPY api/requirements.txt ./api/requirements.txt
RUN pip install --no-cache-dir -r api/requirements.txt

# Copy source, data, and build the C binary
COPY . .
RUN make

ENV PORT=8000
EXPOSE 8000

# Run uvicorn serving the FastAPI app + static UI
CMD ["uvicorn", "api.main:app", "--host", "0.0.0.0", "--port", "8000"]
