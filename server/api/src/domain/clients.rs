use std::sync::Arc;
use axum::body::Body;
use axum::extract::State;
use axum::http;
use axum::http::{Response, StatusCode};
use axum::response::IntoResponse;
use crate::core::io::send_client_message;
use crate::error::ApiError;
use crate::orm::clients::list_client_db;
use crate::state::AppState;

pub async fn get_connected_clients(State(state): State<Arc<AppState>>) -> Result<Response<Body>, ApiError> {
  let response_builder = Response::builder().header(http::header::CONTENT_TYPE, "application/json");
  let clients = state.clients.lock().await.clone();
  if clients.0 {
    return Ok(response_builder.status(StatusCode::SERVICE_UNAVAILABLE).body(Body::from(""))?);
  }

  let response_body = Body::from(serde_json::to_string(&clients.1)?);
  let response = response_builder.body(response_body)?;

  Ok(response)
}

pub async fn update_clients(State(state): State<Arc<AppState>>) -> Result<impl IntoResponse, ApiError> {
  let status = send_client_message(Arc::clone(&state.socket_core)).await;
  match status {
    Ok(ref c) if c == &StatusCode::SERVICE_UNAVAILABLE => Ok(StatusCode::SERVICE_UNAVAILABLE),
    Ok(_) => Ok(StatusCode::OK),
    Err(_) => Ok(StatusCode::INTERNAL_SERVER_ERROR),
  }
}

pub async fn get_client_db(State(_state): State<Arc<AppState>>) -> Result<Response<Body>, ApiError> {
  let response_builder = Response::builder().header(http::header::CONTENT_TYPE, "application/json");
  let clients: Vec<entity::entities::client::Model> = list_client_db().await?;

  let response_body = Body::from(serde_json::to_string(&clients)?);
  let response = response_builder.body(response_body)?;

  Ok(response)
}
